#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class JWTManager
{
private:
    JWTManager() = default;
    
    JWTManager(const JWTManager&) = delete;
    JWTManager& operator=(const JWTManager&) = delete;

    std::string base64UrlEncode(const std::string &input);
    std::string hmacSHA256(const std::string &key, const std::string &data);
    std::string base64_encode(const std::string &input);
    std::string base64_decode(const std::string &input);

public:
    static JWTManager& getInstance() {
        static JWTManager instance;
        return instance;
    }

    std::string generateToken(const json &payload, const std::string &secret);

    json verifySignatureAndDecode(const std::string &token, const std::string &secret);
    
    json formatErrorResponse(const std::string &message);
};