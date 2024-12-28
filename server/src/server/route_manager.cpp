#include <server/route_manager.hpp>
#include <utils/logger.hpp>
#include <sstream>
#include <iostream>

std::map<std::string, std::map<std::string, RouteHandler>> RouteManager::routes;

void RouteManager::addRoute(const std::string &path, const std::string &method, RouteHandler handler)
{
    routes[path][method] = handler;
    Logger::info({"Registered route: " + method + " " + path});
}

bool RouteManager::handleRequest(
    const http::request<http::string_body> &req,
    http::response<http::string_body> &res)
{
    auto [path, queryString] = splitPathAndQuery(std::string(req.target()));
    std::string method = std::string(req.method_string());
    Logger::debug({"Handling request: " + method + " " + path + " with query: " + queryString});

    // Parse query parameters
    QueryParams params = parseQueryParameters(queryString);
    // First try exact match
    if (routes.count(path) > 0 && routes[path].count(method) > 0)
    {
        routes[path][method](req, res);
        return true;
    }
    if (routes.count(path) > 0 && routes[path].count(method) > 0)
    {
        routes[path][method](req, res);
        return true;
    }

    // Try pattern matching for paths with parameters
    for (const auto &route : routes)
    {
        std::regex pattern(route.first);
        if (std::regex_match(path, pattern))
        {
            if (route.second.count(method) > 0)
            {
                route.second.at(method)(req, res);
                return true;
            }
        }
    }

    return false;
}

std::pair<std::string, std::string> RouteManager::splitPathAndQuery(const std::string &target)
{
    size_t pos = target.find('?');
    if (pos == std::string::npos)
    {
        return {target, ""};
    }
    return {target.substr(0, pos - 1), target.substr(pos + 1)};
}

QueryParams RouteManager::parseQueryParameters(const std::string &queryString)
{
    QueryParams params;
    std::stringstream ss(queryString);
    std::string pair;

    while (std::getline(ss, pair, '&'))
    {
        if (pair.empty())
            continue;

        size_t pos = pair.find('=');
        if (pos != std::string::npos)
        {
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            params[key] = value;
        }
    }

    return params;
}
