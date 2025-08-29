#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <iomanip>
#include <sstream>

namespace pme
{
    enum LogLevel
    {
        Success,
        Warning,
        Error
    };

    class Logger
    {
    public:
    public:
        static void Log(LogLevel level, std::string message);

    private:
        static std::string GetCurrentTimeString();
    };
}