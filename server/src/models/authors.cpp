#include <models/authors.hpp>
#include <db/db_manager.hpp>
#include <pqxx/pqxx>
#include <utils/logger.hpp>
#include <utils/SQLBuilder.hpp>
#include <stdexcept>
#include <functional>

Author::Author(const std::string &name, const std::string &email, const std::string &password)
    : name(name), email(email), is_deleted(false), password(password)
{
    id = -1;
}

void Author::setName(const std::string &newName)
{
    name = newName;
}

void Author::setEmail(const std::string &newEmail)
{
    email = newEmail;
}

void Author::setPassword(const std::string &newPassword)
{
    std::string hashedPassword = std::to_string(std::hash<std::string>{}(newPassword));
    password = hashedPassword;
}

void Author::setDeleted(bool status)
{
    is_deleted = status;
}

bool Author::save()
{
    auto conn = DatabaseManager::getInstance().getConnection();
    pqxx::work txn(*conn);
    SQLBuilder builder;
    try
    {
        if (id == -1)
        {
            std::vector<std::string> columns = {"name", "email", "password"};
            std::vector<std::string> values = {name, email, password};
            std::string sql = builder.insert("authors", columns, values).returning({"id"}).build();
            auto result = txn.exec(sql);
            if (result.empty() || result[0].empty())
            {
                throw std::runtime_error("Insert did not return an ID");
            }
            id = result[0][0].as<int>();
        }
        else
        {
            std::vector<std::pair<std::string, std::string>> updates = {
                {"name", name},
                {"email", email},
                {"password", password}};

            std::string updateSql = builder
                                        .update("authors")
                                        .set(updates)
                                        .where({{"id", std::to_string(id), "="}})
                                        .build();

            txn.exec(updateSql);
        }
        if (id != -1) {
            // Update existing documents
            for (const auto& doc : documents) {
                doc->setAuthorId(id);
                doc->save();
            }
        }
        txn.commit();
        Logger::info({"Author saved successfully with ID: " + std::to_string(id)});
        return true;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to save author: " + std::string(e.what())});
        return false;
    }
}

bool Author::remove()
{
    auto conn = DatabaseManager::getInstance().getConnection();
    pqxx::work txn(*conn);
    SQLBuilder builder;
    try
    {
        std::string sql = builder.query("UPDATE authors SET is_deleted = true WHERE id = " + std::to_string(id)).build();
        txn.exec(sql);
        for (const auto& doc : documents)
        {
            std::string deleteDocQuery = builder.deleteFrom("documents").where({{"id", std::to_string(doc->getId()), "="}}).build();
            txn.exec(deleteDocQuery);
        }
        txn.commit();
        Logger::info({"Author removed successfully with ID: " + std::to_string(id)});
        return true;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to remove author: " + std::string(e.what())});
        return false;
    }
}

std::vector<Author> Author::search(const std::string &query)
{
    std::vector<Author> authors;
    try
    {
        auto conn = DatabaseManager::getInstance().getConnection();
        pqxx::work txn(*conn);
        SQLBuilder builder;
        std::string sql = builder.select({"*"}).from("authors").where({{"name", query, "LIKE"}}).build();
        auto result = txn.exec(sql);
        txn.commit();
        for (auto row : result)
        {
            Author author(row["name"].as<std::string>(), row["email"].as<std::string>(), "");
            author.id = row["id"].as<int>();
            author.is_deleted = row["is_deleted"].as<bool>();
            author.populateDocuments();
            authors.push_back(author);
        }
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to search authors: " + std::string(e.what())});
    }
    return authors;
}

std::vector<Author> Author::all()
{
    std::vector<Author> authors;
    try
    {
        auto conn = DatabaseManager::getInstance().getConnection();
        pqxx::work txn(*conn);
        SQLBuilder builder;
        std::string sql = builder.select({"*"}).from("authors").where({{"is_deleted", "false", "="}}).build();
        auto result = txn.exec(sql);
        txn.commit();
        for (auto row : result)
        {
            Author author(row["name"].as<std::string>(), row["email"].as<std::string>(), "");
            author.id = row["id"].as<int>();
            author.is_deleted = row["is_deleted"].as<bool>();
            authors.push_back(author);
        }
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to get all authors: " + std::string(e.what())});
    }
    return authors;
}

Author Author::findById(int id)
{
    try
    {
        auto conn = DatabaseManager::getInstance().getConnection();
        pqxx::work txn(*conn);
        SQLBuilder builder;
        std::string sql = builder.select({"*"}).from("authors").where({{"id", std::to_string(id), "="}}).build();
        auto result = txn.exec(sql);
        txn.commit();
        if (result.empty() || result[0].empty())
        {
            throw std::runtime_error("Author not found");
        }
        auto row = result[0];
        Author author(row["name"].as<std::string>(), row["email"].as<std::string>(), "");
        author.id = row["id"].as<int>();
        author.is_deleted = row["is_deleted"].as<bool>();
        author.populateDocuments();
        return author;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to find author by ID: " + std::string(e.what())});
        return Author("", "", "");
    }
}

Author Author::findByEmail(const std::string &email)
{
    try
    {
        auto conn = DatabaseManager::getInstance().getConnection();
        pqxx::work txn(*conn);
        SQLBuilder builder;
        std::string sql = builder.select({"*"}).from("authors").where({{"email", email, "="}}).build();
        auto result = txn.exec(sql);
        txn.commit();
        if (result.empty() || result[0].empty())
        {
            throw std::runtime_error("Author not found");
        }
        auto row = result[0];
        Author author(row["name"].as<std::string>(), row["email"].as<std::string>(), row["password"].as<std::string>());
        author.id = row["id"].as<int>();
        author.is_deleted = row["is_deleted"].as<bool>();
        author.populateDocuments();
        return author;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to find author by email: " + std::string(e.what())});
        return Author("", "", "");
    }
}

const std::vector<std::shared_ptr<Document>>& Author::getDocuments() const
{
    if (documents.empty()) {
        const_cast<Author*>(this)->populateDocuments();
    }
    return documents;
}

void Author::populateDocuments() {
    auto conn = DatabaseManager::getInstance().getConnection();
    pqxx::work txn(*conn);
    SQLBuilder builder;
    std::string sql = builder.select({"*"}).from("documents").where({{"author_id", std::to_string(id), "="}}).build();
    auto result = txn.exec(sql);

    documents.clear();
    for (auto row : result) {
        auto doc = std::make_shared<Document>(
            row["title"].as<std::string>(),
            row["content"].as<std::string>(),
            std::to_string(id));  // Use author_id instead of owner
        doc->setId(row["id"].as<int>());
        doc->setAuthorId(id);
        doc->setPublic(row["is_public"].as<bool>());
        // Set other document properties as needed
        documents.push_back(doc);
    }
    txn.commit();
}