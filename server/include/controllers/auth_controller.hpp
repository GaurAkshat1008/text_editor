#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <utils/jwtManger.hpp>

using json = nlohmann::json;

class AuthController
{
public:
    AuthController() = default;

    json registerUser(const json &userData);

    json loginUser(const json &userData);

    json me(const std::string &token);

private:
    std::string hashPassword(const std::string &password);

    json formatErrorResponse(const std::string &message);
    JWTManager &jwtManager = JWTManager::getInstance();
};