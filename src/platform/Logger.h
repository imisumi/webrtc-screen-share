#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

enum class LogLevel
{
    Info,
    Warning,
    Error
};

class Logger
{
public:
    static void Log(LogLevel level, const std::string& message)
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::tm timeinfo;
#ifdef _WIN32
        localtime_s(&timeinfo, &time_t);
#else
        localtime_r(&time_t, &timeinfo);
#endif
        
        std::ostringstream oss;
        oss << "[" << std::put_time(&timeinfo, "%H:%M:%S") << "] ";
        
        switch (level)
        {
            case LogLevel::Info:    oss << "[INFO] "; break;
            case LogLevel::Warning: oss << "[WARN] "; break;
            case LogLevel::Error:   oss << "[ERROR] "; break;
        }
        
        oss << message << std::endl;
        std::cout << oss.str();
    }
    
    static void Info(const std::string& message) { Log(LogLevel::Info, message); }
    static void Warning(const std::string& message) { Log(LogLevel::Warning, message); }
    static void Error(const std::string& message) { Log(LogLevel::Error, message); }
};