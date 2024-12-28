#include <server/http_server.hpp>
#include <iostream>
#include <server/route_manager.hpp>
#include <utils/logger.hpp>

HttpServer::HttpServer(
    net::io_context &io_context,
    unsigned short port,
    unsigned int thread_count) : io_context_(io_context),
                                 acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)),
                                 thread_count_(thread_count)
{
    do_accept();
}

void HttpServer::run()
{
    // Spawn worker threads
    for (unsigned int i = 0; i < thread_count_; ++i)
    {
        worker_threads_.emplace_back([this]()
                                     { io_context_.run(); });
    }

    // Wait for all threads to complete
    for (auto &thread : worker_threads_)
    {
        thread.join();
    }
}

void HttpServer::stop()
{
    io_context_.stop();
    for (auto &thread : worker_threads_)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void HttpServer::do_accept()
{
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                // Create session with moved socket
                std::make_shared<HttpSession>(std::move(socket))->start();
            }

            // Continue accepting new connections
            do_accept();
        });
}

// HttpSession Implementation
HttpServer::HttpSession::HttpSession(tcp::socket &&socket)
    : socket_(std::move(socket)) {}

void HttpServer::HttpSession::start()
{
    read_request();
}

void HttpServer::HttpSession::read_request()
{
    auto self = shared_from_this();

    http::async_read(socket_, buffer_, request_,
                     [self](beast::error_code ec, std::size_t bytes_transferred)
                     {
                         if (!ec)
                         {
                             self->process_request();
                         }
                     });
}

void HttpServer::HttpSession::process_request()
{
    if (request_.target() == "/health")
    {
        handle_health_check();
        return;
    }

    if (RouteManager::handleRequest(request_, response_))
    {
        write_response();
        return;
    }

    handle_not_found();
}

void HttpServer::HttpSession::handle_health_check()
{
    response_.version(request_.version());
    response_.result(http::status::ok);
    response_.set(http::field::server, "Boost Beast Server");
    response_.set(http::field::content_type, "application/json");
    response_.body() = R"({"status": "healthy", "message": "Server is running"})";
    response_.prepare_payload();

    write_response();
}

void HttpServer::HttpSession::handle_not_found()
{
    response_.version(request_.version());
    response_.result(http::status::not_found);
    response_.set(http::field::server, "Boost Beast Server");
    response_.set(http::field::content_type, "text/plain");
    response_.body() = "404 Not Found";
    response_.prepare_payload();

    write_response();
}

void HttpServer::HttpSession::write_response()
{
    auto self = shared_from_this();

    http::async_write(socket_, response_,
                      [self](beast::error_code ec, std::size_t bytes_transferred)
                      {
                          self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                      });
}