#include "Utils/include/pme_logger.h"

void pme::Logger::Log(LogLevel level, std::string message)
{
    std::string str_level = "";
    switch (level)
    {
    case 0:
        str_level = "Success";
        break;

    case 1:
        str_level = "Warning";
        break;

    case 2:
        str_level = "Error";
        break;
    }
    std::string time = GetCurrentTimeString();

    std::string str_log = time + " " + str_level + ": " + message;
    
    std::cout << str_log << std::endl;
}

std::string pme::Logger::GetCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *local_time = std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << local_time->tm_hour << ":"
        << std::setw(2) << local_time->tm_min << ":"
        << std::setw(2) << local_time->tm_sec;

    return oss.str();
}