#include <routes/auth_routes.hpp>
#include <boost/beast/http.hpp>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <utils/jwtManger.hpp>
#include <config/app_config.hpp>
#include <utils/logger.hpp>
#include <server/route_manager.hpp>

namespace http = boost::beast::http;
using request = http::request<http::string_body>;
using response = http::response<http::string_body>;
using json = nlohmann::json;

AuthController AuthRoutes::authController;

void AuthRoutes::handleRegisterUser(const request &req, response &res)
{
    try
    {
        Logger::debug({"Registering user"});
        json data = json::parse(req.body());
        json result = authController.registerUser(data);

        if (result.find("error") != result.end())
        {
            throw std::runtime_error(result.dump());
        }

        std::string token = JWTManager::getInstance().generateToken(result, AppConfig::getInstance().getSecretKey());

        res.result(http::status::created);
        res.set(http::field::set_cookie, "token=" + token);
        res.set(http::field::content_type, "application/json");
        res.body() = json{{"message", "User registered successfully"}}.dump();
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error in handleRegisterUser: " + std::string(e.what())});
        res.result(http::status::bad_request);
        res.body() = json{{"error", e.what()}}.dump();
    }
}

void AuthRoutes::handleLoginUser(const request &req, response &res)
{
    try
    {
        Logger::debug({"Logging in user"});
        json data = json::parse(req.body());
        json result = authController.loginUser(data);
        if (result.find("error") != result.end())
        {
            throw std::runtime_error(result.dump());
        }
        std::string token = JWTManager::getInstance().generateToken(result, AppConfig::getInstance().getSecretKey());

        res.result(http::status::ok);
        res.set(http::field::set_cookie, "token=" + token);
        res.set(http::field::content_type, "application/json");
        res.body() = result.dump();
    }
    catch (std::exception &e)
    {
        Logger::error({"error in logging in", e.what()});
        res.result(http::status::bad_request);
        res.body() = json{{"error", e.what()}}.dump();
    }
}

void AuthRoutes::handleLogoutUser(const request &req, response &res)
{
    try
    {
        Logger::debug({"Logging out user"});
        res.result(http::status::ok);
        res.set(http::field::set_cookie, "token=; Max-Age=0");
        res.set(http::field::content_type, "application/json");
        res.body() = json{{"message", "User logged out successfully"}}.dump();
    }
    catch (std::exception &e)
    {
        Logger::debug({"error in logging out", e.what()});
        res.result(http::status::bad_request);
        res.body() = json{{"error", e.what()}}.dump();
    }
}

void AuthRoutes::handleMe(const request &req, response &res)
{
    try
    {
        Logger::debug({"Getting user data"});
        std::string token = req.at(http::field::cookie);
        token = token.substr(token.find("token=") + 6);
        json result = authController.me(token);
        if (result.find("error") != result.end())
        {
            throw std::runtime_error(result.dump());
        }
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = result.dump();
    }
    catch (std::exception &e)
    {
        res.result(http::status::bad_request);
        res.body() = json{{"error", e.what()}}.dump();
    }
}

void AuthRoutes::registerRoutes()
{
    Logger::info({"Registering auth routes"});
    RouteManager::addRoute("/auth/register", "POST", AuthRoutes::handleRegisterUser);
    RouteManager::addRoute("/auth/login", "POST", AuthRoutes::handleLoginUser);
    RouteManager::addRoute("/auth/logout", "DELETE", AuthRoutes::handleLogoutUser);
    RouteManager::addRoute("/auth/me", "GET", AuthRoutes::handleMe);
}