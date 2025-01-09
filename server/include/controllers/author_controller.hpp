#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <models/authors.hpp>

using json = nlohmann::json;

class AuthorController
{
public:
    AuthorController();

    json createAuthor(const std::string &name, const std::string &email, const std::string &password);
    json getAuthor(int id);
    json getAuthor(const std::string &email, bool send_password = false);
    json updateAuthor(int id, const json &updates);
    json deleteAuthor(int id);
    json searchAuthors(const std::string &query);

private:
    json formatAuthorResponse(const Author &author, bool send_password = false);
    json formatErrorResponse(const std::string &message);
};