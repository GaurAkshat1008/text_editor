#pragma once
#include <string>
#include <vector>
#include <pqxx/pqxx>
#include "db_manager.hpp"

class DatabaseMigration {
public:
    static void runMigrations();

private:
    static std::string readMigrationFile(const std::string& filepath);
    static void executeMigration(pqxx::connection& conn, const std::string& sql);
    static void createMigrationsTable(pqxx::connection& conn);
    static bool hasMigrationBeenExecuted(pqxx::connection& conn, const std::string& filename);
    static void recordMigration(pqxx::connection& conn, const std::string& filename);
};
