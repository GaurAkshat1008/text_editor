#pragma once

#include <vector>
#include <string>

class Logger
{
public:
    static Logger &getInstance()
    {
        static Logger instance;
        return instance;
    };
    static void setLogLevel(int logLevel);
    static void info(const std::vector<std::string> &messages);
    static void error(const std::vector<std::string> &messages);
    static void debug(const std::vector<std::string> &messages);
    static void warn(const std::vector<std::string> &messages);

private:
    static int logLevel;
    Logger() = default;
    ~Logger() = default;

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
};