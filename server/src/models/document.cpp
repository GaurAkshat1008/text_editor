#include <models/document.hpp>
#include <utils/logger.hpp>
#include <db/db_manager.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <pqxx/pqxx>
#include <utils/SQLBuilder.hpp>

using json = nlohmann::json;

Document::Document(const std::string &title, const std::string &content, const std::string &owner)
    : title(title), content(content), owner(owner), is_public(false)
{
    // Set initial timestamps
    time(&created_at);
    updated_at = created_at;
    id = -1; // Invalid ID until saved to database
}

void Document::setTitle(const std::string &newTitle)
{
    title = newTitle;
    updateTimestamp();
}

void Document::setContent(const std::string &newContent)
{
    content = newContent;
    updateTimestamp();
}

void Document::setOwner(const std::string &newOwner)
{
    owner = newOwner;
    updateTimestamp();
}

void Document::setPublic(bool status)
{
    is_public = status;
    updateTimestamp();
}

void Document::updateTimestamp()
{
    time(&updated_at);
}

std::string Document::toJson() const
{
    json j = {
        {"id", id},
        {"title", title},
        {"content", content},
        {"owner", owner},
        {"created_at", created_at},
        {"updated_at", updated_at},
        {"is_public", is_public}};
    return j.dump();
}

Document Document::fromJson(const std::string &jsonStr)
{
    try
    {
        json j = json::parse(jsonStr);
        Document doc(j["title"], j["content"], j["owner"]);
        doc.setId(j["id"]);
        doc.is_public = j["is_public"];
        doc.created_at = j["created_at"];
        doc.updated_at = j["updated_at"];
        return doc;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error parsing document from JSON: " + std::string(e.what())});
        throw;
    }
}

bool Document::save()
{
    try
    {
        auto conn = DatabaseManager::getInstance().getConnection();
        pqxx::work txn(*conn);
        SQLBuilder builder;
        if (id == -1)
        {
            std::vector<std::string> columns = {
                "title", "content", "owner", "created_at", "updated_at", "is_public"};
            std::vector<std::string> values = {
                "'" + title + "'", "'" + content + "'", "'" + owner + "'", "to_timestamp(" + std::to_string(created_at) + ")",
                "to_timestamp(" + std::to_string(updated_at) + ")",
                is_public ? "true" : "false"};

            std::string sql = builder
                .insert("documents", columns, values)
                .returning("id")  // Add this line
                .build();

            Logger::debug({"Executing SQL: " + sql});
            auto result = txn.exec(sql);
            
            if (result.empty() || result[0].empty()) {
                throw std::runtime_error("Insert did not return an ID");
            }
            
            id = result[0][0].as<int>();
        }
        else
        {
            std::string updateSql = builder.query("UPDATE documents SET title = '" + title + "', content = '" + content + "', owner = '" + owner + "', updated_at = to_timestamp(" + std::to_string(updated_at) + "), is_public = " + (is_public ? "true" : "false") + " WHERE id = " + std::to_string(id)).build();

            txn.exec(updateSql);
        }

        txn.commit();
        Logger::info({"Document saved successfully with ID: " + std::to_string(id)});
        return true;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to save document: " + std::string(e.what())});
        return false;
    }
}

bool Document::remove()
{
    try
    {
        auto conn = DatabaseManager::getInstance().getConnection();
        pqxx::work txn(*conn);

        SQLBuilder builder;

        std::string sql = builder.deleteFrom("documents").where({{"id", std::to_string(id), "="}}).build();
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to remove document: " + std::string(e.what())});
        return false;
    }
}

std::vector<Document> Document::search(
    const std::string &query, const std::string &owner, bool includePrivate)
{
    try
    {
        auto conn = DatabaseManager::getInstance().getConnection();
        pqxx::work txn(*conn);
        SQLBuilder builder;
        std::string sql = builder.select({"*"}).from("documents").where({{"title", query, "LIKE"}}).where({{"owner", owner, "="}}).where({{"is_public", "true", "="}}).build();
        auto result = txn.exec(sql);

        std::vector<Document> documents;
        for (auto row : result)
        {
            Document doc(row["title"].as<std::string>(), row["content"].as<std::string>(), row["owner"].as<std::string>());
            doc.setId(row["id"].as<int>());
            doc.created_at = row["created_at"].as<time_t>();
            doc.updated_at = row["updated_at"].as<time_t>();
            doc.is_public = row["is_public"].as<bool>();
            documents.push_back(doc);
        }

        txn.commit();
        return documents;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to search documents: " + std::string(e.what())});
        return {};
    }
}

std::shared_ptr<Document> Document::findById(int id)
{
    try
    {
        auto conn = DatabaseManager::getInstance().getConnection();
        pqxx::work txn(*conn);

        SQLBuilder builder;
        std::string sql = builder.select({"*"}).from("documents").where({{"id", std::to_string(id), "="}}).build();
        auto result = txn.exec(sql);

        if (result.empty())
        {
            Logger::warn({"Document not found with ID: " + std::to_string(id)});
            return nullptr;
        }

        auto row = result[0];
        auto doc = std::make_shared<Document>(row["title"].as<std::string>(), row["content"].as<std::string>(), row["owner"].as<std::string>());
        doc->setId(row["id"].as<int>());
        doc->created_at = row["created_at"].as<time_t>();
        doc->updated_at = row["updated_at"].as<time_t>();
        doc->is_public = row["is_public"].as<bool>();

        txn.commit();
        return doc;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to find document: " + std::string(e.what())});
        return nullptr;
    }
}