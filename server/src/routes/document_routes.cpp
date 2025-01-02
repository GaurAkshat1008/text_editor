#include <routes/document_routes.hpp>
#include <utils/logger.hpp>
#include <nlohmann/json.hpp>
#include <server/route_manager.hpp>

using json = nlohmann::json;

DocumentController DocumentRoutes::documentController;

void DocumentRoutes::handleGetDocument(
    const http::request<http::string_body> &req,
    http::response<http::string_body> &res)
{
    try
    {
        Logger::debug({"Getting document with ID: ", req.target()});
        // int id = std::stoi(req.target().to_string());
        int id = 1;
        auto document = documentController.getDocument(id);

        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = document;
    }
    catch (const std::exception &e)
    {
        res.result(http::status::internal_server_error);
        res.body() = json{{"error", e.what()}}.dump();
    }
    res.prepare_payload();
}

void DocumentRoutes::handleCreateDocument(
    const http::request<http::string_body> &req,
    http::response<http::string_body> &res)
{
    try
    {
        auto data = json::parse(req.body());
        documentController.createDocument(
            data["title"],
            data["content"],
            data["owner"]);

        res.result(http::status::created);
        res.set(http::field::content_type, "application/json");
        res.body() = json{{"message", "Document created successfully"}}.dump();
    }
    catch (const std::exception &e)
    {
        res.result(http::status::bad_request);
        res.body() = json{{"error", e.what()}}.dump();
    }
    res.prepare_payload();
}

void DocumentRoutes::handleUpdateDocument(
    const http::request<http::string_body> &req,
    http::response<http::string_body> &res)
{
    try
    {
        auto data = json::parse(req.body());
        auto [path, queryString] = RouteManager::splitPathAndQuery(std::string(req.target()));
        QueryParams params = RouteManager::parseQueryParameters(queryString);
        int id = std::stoi(params["id"]);
        auto result = documentController.updateDocument(id, data);

        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = result.dump();
    }
    catch (const std::exception &e)
    {
        res.result(http::status::bad_request);
        res.body() = json{{"error", e.what()}}.dump();
    }
    res.prepare_payload();
}

void DocumentRoutes::handleDeleteDocument(
    const http::request<http::string_body> &req,
    http::response<http::string_body> &res)
{
    try
    {
        int id = 1; // TODO: Extract ID from URL params
        auto result = documentController.deleteDocument(id);

        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = result.dump();
    }
    catch (const std::exception &e)
    {
        res.result(http::status::bad_request);
        res.body() = json{{"error", e.what()}}.dump();
    }
    res.prepare_payload();
}

void DocumentRoutes::handleSearchDocuments(
    const http::request<http::string_body> &req,
    http::response<http::string_body> &res)
{
    try
    {
        auto [path, queryString] = RouteManager::splitPathAndQuery(std::string(req.target()));
        auto params = RouteManager::parseQueryParameters(queryString);

        std::string query = params["q"];
        std::string owner = params["owner"];
        if (query.empty())
        {
            throw std::invalid_argument("Query parameter 'q' is required");
        }
        auto result = documentController.searchDocuments(query, owner);

        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = result.dump();
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to search documents: " + std::string(e.what())});
        res.result(http::status::bad_request);
        res.body() = json{{"error", e.what()}}.dump();
    }
    res.prepare_payload();
}

void DocumentRoutes::registerRoutes()
{
    Logger::info({"Registering document routes"});
    RouteManager::addRoute("/api/documents/search", "GET", handleSearchDocuments);
    RouteManager::addRoute("/api/documents", "GET", handleGetDocument);
    RouteManager::addRoute("/api/documents", "POST", handleCreateDocument);
    RouteManager::addRoute("/api/documents", "PUT", handleUpdateDocument);
    RouteManager::addRoute("/api/documents", "DELETE", handleDeleteDocument);
}
