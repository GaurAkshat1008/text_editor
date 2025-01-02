#include <boost/asio.hpp>
#include <server/http_server.hpp>
#include <db/db_manager.hpp>
#include <config/app_config.hpp>
#include <utils/logger.hpp>
#include <routes/document_routes.hpp>
#include <db/db_migration.hpp>

#include <csignal>
#include <memory>
#include <stdexcept>

std::unique_ptr<HttpServer> g_server;

void signalHandler(int signal)
{
    if (g_server)
    {
        Logger::warn({"Shutting down server"});
        g_server->stop();
        exit(signal);
    }
}

int main()
{
    try
    {
        auto &config = AppConfig::getInstance();
        Logger::getInstance().setLogLevel(config.isDebugModeEnabled() ? 4 : 3);
        try
        {
            DatabaseManager::getInstance().initialize(5);
            DatabaseMigration::runMigrations();
        }
        catch (std::exception &e)
        {
            Logger::error({"Error initializing database: " + std::string(e.what())});
            return 1;
        }

        boost::asio::io_context io_context;

        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        // Register routes
        DocumentRoutes::registerRoutes();

        g_server = std::make_unique<HttpServer>(
            io_context,
            config.getPort());

        Logger::info({"Server starting on port " + std::to_string(config.getPort())});

        g_server->run();
    }
    catch (const std::exception &e)
    {
        Logger::error({"An error occurred: " + std::string(e.what())});
        return 1;
    }

    return 0;
}