#include "db/db_manager.hpp"
#include <stdexcept>
#include <iostream>
#include "utils/logger.hpp"

void DatabaseManager::initialize(size_t poolSize)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_initialized)
    {
        return;
    }

    try
    {
        for (size_t i = 0; i < poolSize; i++)
        {
            m_connectionPool.push(createConnection());
        }

        m_initialized = true;
        Logger::info({"Database connection pool initialized with " + std::to_string(poolSize) + " connections"});
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error initializing database connection pool: " << e.what() << std::endl;
        throw;
    }
}

std::shared_ptr<pqxx::connection> DatabaseManager::getConnection()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_initialized)
    {
        throw std::runtime_error("DatabaseManager not initialized");
    }

    if (m_connectionPool.empty())
    {
        return createConnection();
    }

    auto connection = m_connectionPool.front();
    m_connectionPool.pop();

    try
    {
        if (!connection->is_open())
        {
            connection = createConnection();
        }
    }
    catch (const std::exception &)
    {
        connection = createConnection();
    }

    return connection;
}

void DatabaseManager::releaseConnection(std::shared_ptr<pqxx::connection> connection)
{
    if (!connection)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_initialized)
    {
        throw std::runtime_error("DatabaseManager not initialized");
    }

    try
    {

        if (connection->is_open())
        {
            m_connectionPool.push(connection);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error releasing connection to pool: " << e.what() << std::endl;
    }
}

void DatabaseManager::shutdown()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_initialized)
    {
        return;
    }

    while (!m_connectionPool.empty())
    {
        auto connection = m_connectionPool.front();
        m_connectionPool.pop();

        try
        {
            if (connection->is_open())
            {
                connection->close();
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error disconnecting database connection: " << e.what() << std::endl;
        }
    }

    m_initialized = false;
}

std::shared_ptr<pqxx::connection> DatabaseManager::createConnection()
{
    try
    {
        auto &config = AppConfig::getInstance();
        return std::make_shared<pqxx::connection>(
            config.getDatabaseConnectionString());
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(
            std::string("Failed to create database connection: ") + e.what());
    }
}