#include "db/db_migration.hpp"
#include "utils/logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utils/sqlbuilder.hpp>

namespace fs = std::filesystem;

void DatabaseMigration::runMigrations()
{
    try
    {
        auto conn = DatabaseManager::getInstance().getConnection();
        createMigrationsTable(*conn);

        // Get paths for both source and build migrations
        std::string buildMigrationsPath = fs::current_path().string() + "/migrations";
        std::string sourceMigrationsPath = fs::current_path().parent_path().string() + "/migrations";

        Logger::debug({"Source migrations path: " + sourceMigrationsPath});
        Logger::debug({"Build migrations path: " + buildMigrationsPath});

        // Create build migrations directory if it doesn't exist
        if (!fs::exists(buildMigrationsPath))
        {
            Logger::info({"Creating migrations directory in build folder"});
            fs::create_directory(buildMigrationsPath);
        }

        // Copy files from source to build if source exists
        if (fs::exists(sourceMigrationsPath))
        {
            Logger::info({"Copying migration files from source to build"});
            for (const auto &entry : fs::directory_iterator(sourceMigrationsPath))
            {
                if (entry.path().extension() == ".sql")
                {
                    fs::copy_file(
                        entry.path(),
                        buildMigrationsPath + "/" + entry.path().filename().string(),
                        fs::copy_options::overwrite_existing);
                    Logger::debug({"Copied migration file: " + entry.path().filename().string()});
                }
            }
        }

        // Now execute migrations from build directory
        for (const auto &entry : fs::directory_iterator(buildMigrationsPath))
        {
            if (entry.path().extension() == ".sql")
            {
                std::string filename = entry.path().filename().string();
                try
                {
                    if (!hasMigrationBeenExecuted(*conn, filename))
                    {
                        Logger::info({"Executing migration: " + filename});
                        std::string sql = readMigrationFile(entry.path().string());
                        executeMigration(*conn, sql);
                        recordMigration(*conn, filename);
                        Logger::info({"Successfully executed migration: " + filename});
                    }
                    else
                    {
                        Logger::debug({"Skipping already executed migration: " + filename});
                    }
                }
                catch (const std::exception &e)
                {
                    Logger::error({"Failed to execute migration " + filename + ": " + e.what()});
                    throw;
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to run migrations: " + std::string(e.what())});
        throw;
    }
}

std::string DatabaseMigration::readMigrationFile(const std::string &filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open migration file: " + filepath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void DatabaseMigration::createMigrationsTable(pqxx::connection &conn)
{
    try
    {
        pqxx::work txn(conn);
        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS migrations (
                id SERIAL PRIMARY KEY,
                filename VARCHAR(255) NOT NULL UNIQUE,
                executed_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
            )
        )");
        txn.commit();
        Logger::debug({"Migrations table created or verified"});
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to create migrations table: " + std::string(e.what())});
        throw;
    }
}

bool DatabaseMigration::hasMigrationBeenExecuted(pqxx::connection &conn, const std::string &filename)
{
    try
    {
        pqxx::work txn(conn);
        SQLBuilder builder;
        std::string query = builder.select({"COUNT(*)"}).from("migrations").where({{"filename", filename, "="}}).build();
        auto result = txn.exec_params(query);
        txn.commit();
        if (result.empty() || result[0].empty())
        {
            Logger::error({"Migration count query returned no rows"});
            return false;
        }

        return result[0][0].as<int>() > 0;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to check migration status: " + std::string(e.what())});
        throw;
    }
}

void DatabaseMigration::recordMigration(pqxx::connection &conn, const std::string &filename)
{
    try
    {
        pqxx::work txn(conn);
        SQLBuilder builder;
        std::string query = builder.insert("migrations", {"filename"}, {filename}).build();
        txn.exec_params(query);
        txn.commit();
        Logger::debug({"Recorded migration execution: " + filename});
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to record migration: " + std::string(e.what())});
        throw;
    }
}

void DatabaseMigration::executeMigration(pqxx::connection &conn, const std::string &sql)
{
    try
    {
        pqxx::work txn(conn);
        txn.exec(sql);
        txn.commit();
    }
    catch (const std::exception &e)
    {
        Logger::error({"Failed to execute migration SQL: " + std::string(e.what())});
        throw;
    }
}
