#include <controllers/auth_controller.hpp>
#include <nlohmann/json.hpp>
#include <controllers/author_controller.hpp>
#include <string>
#include <config/app_config.hpp>
#include <utils/logger.hpp>
using json = nlohmann::json;

json AuthController::formatErrorResponse(const std::string &message)
{
    json response;
    response["error"] = message;
    return response;
}

std::string AuthController::hashPassword(const std::string &password)
{
    return std::to_string(std::hash<std::string>{}(password));
}

json AuthController::registerUser(const json &userData)
{
    try
    {
        Logger::info({"Starting user registration"});

        std::string name = userData["name"];
        std::string email = userData["email"];
        std::string password = userData["password"];
        AuthorController authorController;
        json authorResponse = authorController.createAuthor(name, email, password);

        if (authorResponse.find("error") != authorResponse.end())
        {
            return formatErrorResponse(authorResponse.dump());
        }

        Logger::info({"User registration completed for email: " + email});
        return authorResponse;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error in registerUser: " + std::string(e.what())});
        return formatErrorResponse(e.what());
    }
}

json AuthController::loginUser(const json &userData)
{
    try
    {
        Logger::info({"Starting user login"});

        std::string email = userData["email"];
        std::string password = userData["password"];

        AuthorController authorController;
        json authorResponse = authorController.getAuthor(email, true);
        if (authorResponse.find("error") != authorResponse.end())
        {
            return formatErrorResponse(authorResponse.dump());
        }
        Logger::debug({authorResponse.dump()});
        std::string hashedPassword = authorResponse["password"];

        if (hashPassword(password) != hashedPassword)
        {
            return formatErrorResponse("Invalid password");
        }

        Logger::info({"User login completed for email: " + email});
        authorResponse.erase("password");
        return authorResponse;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error in loginUser: " + std::string(e.what())});
        return formatErrorResponse(e.what());
    }
}

json AuthController::me(const std::string &token)
{
    try
    {
        Logger::info({"Processing 'me' request"});

        AuthorController authorController;
        json payload = jwtManager.verifySignatureAndDecode(token, AppConfig::getInstance().getSecretKey());
        int userId = payload["id"];
        json authorResponse = authorController.getAuthor(userId);

        if (authorResponse.find("error") != authorResponse.end())
        {
            return formatErrorResponse("User not found");
        }

        Logger::info({"Returning author data for user ID: " + std::to_string(userId)});
        return authorResponse;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error in me: " + std::string(e.what())});
        return formatErrorResponse(e.what());
    }
}