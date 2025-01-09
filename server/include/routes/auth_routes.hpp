#pragma once
#include <boost/beast/http.hpp>
#include <controllers/auth_controller.hpp>

namespace http = boost::beast::http;
using request = http::request<http::string_body>;
using response = http::response<http::string_body>;

class AuthRoutes
{
public:
    static void registerRoutes();

    static void handleRegisterUser(const request &req, response &res);
    static void handleLoginUser(const request &req, response &res);
    static void handleLogoutUser(const request &req, response &res);
    static void handleMe(const request &req, response &res);

private:
    static AuthController authController;
};
