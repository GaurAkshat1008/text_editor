#pragma once
#include <boost/beast/http.hpp>
#include <controllers/document_controller.hpp>

namespace http = boost::beast::http;

class DocumentRoutes
{
public:
    static void registerRoutes();

    // Route handlers
    static void handleGetDocument(const http::request<http::string_body> &req, http::response<http::string_body> &res);
    static void handleCreateDocument(const http::request<http::string_body> &req, http::response<http::string_body> &res);
    static void handleUpdateDocument(const http::request<http::string_body> &req, http::response<http::string_body> &res);
    static void handleDeleteDocument(const http::request<http::string_body> &req, http::response<http::string_body> &res);
    static void handleSearchDocuments(const http::request<http::string_body> &req, http::response<http::string_body> &res);

private:
    static DocumentController documentController;
};
