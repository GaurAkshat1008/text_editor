#include <utils/jwtManger.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
#include <algorithm>

using json = nlohmann::json;

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::string JWTManager::base64_encode(const std::string &input)
{
    std::string output;
    int val = 0, valb = -6;
    for (unsigned char c : input)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            output.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
    {
        output.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (output.size() % 4)
    {
        output.push_back('=');
    }
    return output;
}

std::string JWTManager::base64_decode(const std::string &input)
{
    std::string output;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++)
    {
        T[base64_chars[i]] = i;
    }
    int val = 0, valb = -8;
    for (unsigned char c : input)
    {
        if (T[c] == -1)
        {
            break;
        }
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0)
        {
            output.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return output;
}

std::string JWTManager::base64UrlEncode(const std::string &input)
{
    std::string base64 = base64_encode(input);
    std::replace(base64.begin(), base64.end(), '+', '-');
    std::replace(base64.begin(), base64.end(), '/', '_');
    base64.erase(std::remove(base64.begin(), base64.end(), '='), base64.end());
    return base64;
}

std::string JWTManager::hmacSHA256(const std::string &key, const std::string &data)
{
    unsigned char *result;
    unsigned int len = EVP_MAX_MD_SIZE;
    result = (unsigned char *)OPENSSL_malloc(len);

    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key.c_str(), key.length(), EVP_sha256(), NULL);
    HMAC_Update(ctx, (unsigned char *)data.c_str(), data.length());
    HMAC_Final(ctx, result, &len);
    HMAC_CTX_free(ctx);

    std::ostringstream oss;
    for (unsigned int i = 0; i < len; i++)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)result[i];
    }
    OPENSSL_free(result);
    return oss.str();
}

std::string JWTManager::generateToken(const json &payload, const std::string &secret)
{
    json header = {
        {"alg", "HS256"},
        {"typ", "JWT"}};

    json payloadWithExp = payload;
    payloadWithExp["exp"] = time(0) + 60 * 60;
    payloadWithExp["iat"] = time(0);

    std::string headerEncoded = base64UrlEncode(header.dump());
    std::string payloadEncoded = base64UrlEncode(payloadWithExp.dump());

    std::string to_sign = headerEncoded + "." + payloadEncoded;

    std::string signature = base64UrlEncode(hmacSHA256(secret, to_sign));

    return headerEncoded + "." + payloadEncoded + "." + signature;
}

json JWTManager::verifySignatureAndDecode(const std::string &token, const std::string &secret)
{
    std::vector<std::string> parts;
    std::string part;
    std::istringstream tokenStream(token);
    while (std::getline(tokenStream, part, '.'))
    {
        parts.push_back(part);
    }

    if (parts.size() != 3)
    {
        return formatErrorResponse("Invalid token format");
    }

    std::string to_sign = parts[0] + "." + parts[1];
    std::string signature = base64UrlEncode(hmacSHA256(secret, to_sign));

    if (signature != parts[2])
    {
        return formatErrorResponse("Invalid signature");
    }

    std::string payload = parts[1];
    std::string decodedPayload;
    for (size_t i = 0; i < payload.size(); i++)
    {
        if (payload[i] == '_')
        {
            decodedPayload.push_back('/');
        }
        else if (payload[i] == '-')
        {
            decodedPayload.push_back('+');
        }
        else
        {
            decodedPayload.push_back(payload[i]);
        }
    }

    while (decodedPayload.size() % 4)
    {
        decodedPayload.push_back('=');
    }

    std::string decodedPayloadStr = base64_decode(decodedPayload);
    return json::parse(decodedPayloadStr);
}

json JWTManager::formatErrorResponse(const std::string &message)
{
    json response = {
        {"error", message}};
    return response;
}