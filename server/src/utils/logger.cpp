#include <iostream>
#include <vector>
#include <string>
#include <utils/logger.hpp>

int Logger::logLevel = 3;

void Logger::setLogLevel(int logLevel)
{
    Logger::logLevel = logLevel;
}

void Logger::info(const std::vector<std::string> &messages)
{
    for (const auto &message : messages)
    {
        std::cout << "\033[32m[INFO] " << message << "\033[0m" << std::endl; // Green
    }
}

void Logger::error(const std::vector<std::string> &messages)
{
    for (const auto &message : messages)
    {
        std::cerr << "\033[31m[ERROR] " << message << "\033[0m" << std::endl; // Red
    }
}

void Logger::debug(const std::vector<std::string> &messages)
{
    if (logLevel < 4)
        return;
    for (const auto &message : messages)
    {
        std::cout << "\033[34m[DEBUG] " << message << "\033[0m" << std::endl; // Blue
    }
}

void Logger::warn(const std::vector<std::string> &messages)
{
    if (logLevel < 3)
        return;
    for (const auto &message : messages)
    {
        std::cout << "\033[33m[WARN] " << message << "\033[0m" << std::endl; // Yellow
    }
}