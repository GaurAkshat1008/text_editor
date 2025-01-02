#include "utils/timestampConverter.hpp"
#include <sstream>
#include <iomanip>

time_t convertTimestampToTimeT(const std::string& timestamp)
{
    std::istringstream ss(timestamp);
    std::tm tm = {};
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return std::mktime(&tm);
}