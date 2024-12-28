#pragma once
#include <boost/beast/http.hpp>
#include <functional>
#include <map>
#include <string>
#include <regex>

namespace http = boost::beast::http;

using RouteHandler = std::function<void(const http::request<http::string_body>&, http::response<http::string_body>&)>;
using QueryParams = std::map<std::string, std::string>;

class RouteManager {
public:
    static void addRoute(const std::string& path, const std::string& method, RouteHandler handler);
    static bool handleRequest(
        const http::request<http::string_body>& req,
        http::response<http::string_body>& res);
    
    static QueryParams parseQueryParameters(const std::string& queryString);
    static std::pair<std::string, std::string> splitPathAndQuery(const std::string& target);

private:
    static std::map<std::string, std::map<std::string, RouteHandler>> routes;
    static std::string extractPathParam(const std::string& path, const std::string& pattern);
};
