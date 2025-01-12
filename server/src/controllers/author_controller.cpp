#include <controllers/author_controller.hpp>
#include <nlohmann/json.hpp>
#include <models/authors.hpp>
#include <utils/logger.hpp>
#include <string>
#include <controllers/document_controller.hpp>

using json = nlohmann::json;

AuthorController::AuthorController() {}

json AuthorController::createAuthor(const std::string &name, const std::string &email, const std::string &password)
{
    try
    {
        Logger::info({"Creating author: " + name});
        auto existingAuthor = Author::findByEmail(email);
        if (existingAuthor.getId() != -1)
        {
            return formatErrorResponse("Author with email already exists");
        }
        Author author(name, email, password);
        author.setPassword(password);
        if (author.save())
        {
            Logger::info({"Author created: " + name});
            return formatAuthorResponse(author);
        }
        return formatErrorResponse("Failed to create author");
    }
    catch (std::exception &e)
    {
        return formatErrorResponse(e.what());
    }
}

json AuthorController::getAuthor(int id)
{
    try
    {
        auto author = Author::findById(id);
        if (author.getId() == -1)
        {
            return formatErrorResponse("Author not found");
        }
        return formatAuthorResponse(author);
    }
    catch (std::exception &e)
    {
        return formatErrorResponse(e.what());
    }
}

json AuthorController::getAuthor(const std::string &email, bool send_password)
{
    try
    {
        auto author = Author::findByEmail(email);
        if (author.getId() == -1)
        {
            return formatErrorResponse("Author not found");
        }
        return formatAuthorResponse(author, send_password);
    }
    catch (std::exception &e)
    {
        return formatErrorResponse(e.what());
    }
}

json AuthorController::updateAuthor(int id, const json &updates)
{
    auto author = Author::findById(id);
    if (author.getId() == -1)
    {
        return formatErrorResponse("Author not found");
    }

    if (updates.find("name") != updates.end())
    {
        author.setName(updates["name"]);
    }
    if (updates.find("email") != updates.end())
    {
        author.setEmail(updates["email"]);
    }
    if (updates.find("password") != updates.end())
    {
        author.setPassword(updates["password"]);
    }

    if (author.save())
    {
        Logger::info({"Author updated: " + author.getName()});
        return formatAuthorResponse(author);
    }
    return formatErrorResponse("Failed to update author");
}

json AuthorController::deleteAuthor(int id)
{
    auto author = Author::findById(id);
    if (author.getId() == -1)
    {
        return formatErrorResponse("Author not found");
    }

    author.setDeleted(true);
    if (author.save())
    {
        Logger::info({"Author deleted: " + author.getName()});
        return formatAuthorResponse(author);
    }
    return formatErrorResponse("Failed to delete author");
}

json AuthorController::searchAuthors(const std::string &query)
{
    auto authors = Author::search(query);
    json response;
    for (auto &author : authors)
    {
        response.push_back(formatAuthorResponse(author));
    }
    return response;
}

json AuthorController::formatAuthorResponse(const Author &author, bool send_password)
{
    json response;
    response["id"] = author.getId();
    response["name"] = author.getName();
    response["email"] = author.getEmail();
    
    // Convert documents to JSON array
    json documents_json = json::array();
    for (const auto& doc : author.getDocuments()) {
        documents_json.push_back(DocumentController::documentToJson(*doc));
    }
    response["documents"] = documents_json;

    if (send_password)
    {
        response["password"] = author.getPassword();
    }
    return response;
}

json AuthorController::formatErrorResponse(const std::string &message)
{
    json response;
    response["error"] = message;
    return response;
}