#pragma once

#include <pqxx/pqxx>
#include <memory>
#include <string>
#include <mutex>
#include <queue>
#include "config/app_config.hpp"

class DatabaseManager
{
public:
    static DatabaseManager &getInstance()
    {
        static DatabaseManager instance;
        return instance;
    }

    std::shared_ptr<pqxx::connection> getConnection();

    void releaseConnection(std::shared_ptr<pqxx::connection> connection);

    void initialize(size_t poolSize = 5);

    void shutdown();

private:
    DatabaseManager() = default;
    ~DatabaseManager() { shutdown(); }

    // prevent copying the DatabaseManager
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;

    std::queue<std::shared_ptr<pqxx::connection>> m_connectionPool;
    std::mutex m_mutex;
    bool m_initialized = false;

    std::shared_ptr<pqxx::connection> createConnection();
};