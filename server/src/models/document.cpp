#include <models/document.hpp>
#include <utils/logger.hpp>
#include <db/db_manager.hpp>
#include <sstream>
#include <pqxx/pqxx>
#include <utils/SQLBuilder.hpp>
#include <iostream>
#include <iomanip>
#include <utils/timestampConverter.hpp>

Document::Document(const std::string &title, const std::string &content, const std::string &owner)
    : title(title), content(content), is_public(false), author_id(-1)
{
    time(&created_at);
    updated_at = created_at;
    id = -1;
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

void Document::setPublic(bool status)
{
    is_public = status;
    updateTimestamp();
}

void Document::setAuthorId(int newAuthorId)
{
    author_id = newAuthorId;
    updateTimestamp();
}

void Document::updateTimestamp()
{
    time(&updated_at);
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
                "title", "content", "created_at", "updated_at", "is_public", "author_id"};
            std::vector<std::string> values = {
                title, content,
                "to_timestamp(" + std::to_string(created_at) + ")",
                "to_timestamp(" + std::to_string(updated_at) + ")",
                is_public ? "true" : "false",
                std::to_string(author_id)};

            std::string sql = builder
                                  .insert("documents", columns, values)
                                  .returning({"id", "author_id"})
                                  .build();

            Logger::debug({"Executing SQL: " + sql});
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
                {"title", title}, {"content", content}, {"is_public", is_public ? "true" : "false"}, {"author_id", std::to_string(author_id)}};

            std::string updateSql = builder
                                        .update("documents")
                                        .set(updates)
                                        .where({{"id", std::to_string(id), "="}})
                                        .returning({"id", "author_id"})
                                        .build();

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
        auto query_builder = builder.select({"*"}).from("documents").where({{"title", query, "LIKE"}});
        if (!owner.empty())
        {
            query_builder.where({{"owner", owner, "="}});
        }
        if (!includePrivate)
        {
            query_builder.where({{"is_public", "true", "="}});
        }
        std::string sql = query_builder.build();
        auto result = txn.exec(sql);

        std::vector<Document> documents;
        for (auto row : result)
        {
            Document doc(row["title"].as<std::string>(), row["content"].as<std::string>(), row["owner"].as<std::string>());
            doc.setId(row["id"].as<int>());
            doc.created_at = convertTimestampToTimeT(row["created_at"].as<std::string>());
            doc.updated_at = convertTimestampToTimeT(row["updated_at"].as<std::string>());
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
        txn.commit();

        if (result.empty())
        {
            Logger::warn({"Document not found with ID: " + std::to_string(id)});
            return nullptr;
        }

        auto row = result[0];
        auto doc = std::make_shared<Document>(row["title"].as<std::string>(), row["content"].as<std::string>(), row["owner"].as<std::string>());
        doc->setId(row["id"].as<int>());
        doc->created_at = convertTimestampToTimeT(row["created_at"].as<std::string>());
        doc->updated_at = convertTimestampToTimeT(row["updated_at"].as<std::string>());
        doc->is_public = row["is_public"].as<bool>();

        return doc;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to find document: " + std::string(e.what())});
        return nullptr;
    }
}
