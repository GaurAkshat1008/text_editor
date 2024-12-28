#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <memory>
#include <thread>
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class HttpServer
{
public:
    HttpServer(
        net::io_context &io_context,
        unsigned short port,
        unsigned int thread_count = std::thread::hardware_concurrency());

    void run();
    void stop();

private:
    class HttpSession : public std::enable_shared_from_this<HttpSession>
    {
    private:
        tcp::socket socket_;
        beast::flat_buffer buffer_;
        http::request<http::string_body> request_;
        http::response<http::string_body> response_;

    public:
        // Updated constructor to take socket by move
        explicit HttpSession(tcp::socket &&socket);

        void start();

    private:
        void read_request();
        void process_request();
        void handle_health_check();
        void handle_not_found();
        void write_response();
    };

    net::io_context &io_context_;
    tcp::acceptor acceptor_;
    unsigned int thread_count_;
    std::vector<std::thread> worker_threads_;

    void do_accept();
};