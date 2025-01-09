#pragma once

#include <string>
#include <iostream>

class AppConfig
{
public:
    static AppConfig &getInstance()
    {
        static AppConfig instance;
        return instance;
    }

    AppConfig(const AppConfig &) = delete;
    AppConfig &operator=(const AppConfig &) = delete;

    unsigned short getPort() const { return 4000; }
    std::string getDatabaseConnectionString() const
    {
        return "postgresql://postgres:psql@localhost/text_editor";
    }

    bool isDebugModeEnabled() const { return true; }

    std::string getSecretKey()
    {
        return "secret";
    }

private:
    AppConfig() = default;
};