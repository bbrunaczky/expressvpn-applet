#pragma once

#include <string>
#include <chrono>
#include <format>
#include <sstream>
#include <iostream>
#include <syncstream>


/*
 * Lightweight logger
 */

enum class LogLevel
{
    DEBUG,
    INFO,
    ERROR
};


class Logger
{
public:
    Logger(std::string module = "main");

    template <typename... Ts>
    void debug(Ts &&... ts);
    template <typename... Ts>
    void info(Ts &&... ts);
    template <typename... Ts>
    void error(Ts &&... ts);

private:

    constexpr static std::string loglevelToText(LogLevel level);
    
    template <typename... Ts>
    std::string concat(Ts &&... ts) const;

    template <typename... Ts>
    void log(LogLevel level, Ts &&... ts);
    
    std::string _module;
};


template <typename... Ts>
void Logger::debug(Ts &&... ts)
{
    log(LogLevel::DEBUG, std::forward<Ts>(ts)...);
}

template <typename... Ts>
void Logger::info(Ts &&... ts)
{
    log(LogLevel::INFO, std::forward<Ts>(ts)...);
}

template <typename... Ts>
void Logger::error(Ts &&... ts)
{
    log(LogLevel::ERROR, std::forward<Ts>(ts)...);
}


constexpr std::string Logger::loglevelToText(LogLevel level)
{
    switch (level)
    {
        case LogLevel::DEBUG:
            return "DEBUG";
            break;
        case LogLevel::INFO:
            return "INFO";
            break;
        case LogLevel::ERROR:
            return "ERROR";
            break;
        default:
            throw std::runtime_error("Invalid LogLevel!");
    }
}


template <typename... Ts>
std::string Logger::concat(Ts &&... ts) const
{
    bool first{true};
    std::stringstream ss;
    auto add = [&ss, &first] (auto && t)
               {
                   ss << (first ? (first = false, "") : " ") << t;
               };
    (add(ts), ...);
    return ss.str();
}

    
template <typename... Ts>
void Logger::log(LogLevel level, Ts &&... ts)
{
    auto now = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::round<std::chrono::milliseconds>(std::chrono::system_clock::now()));
    auto dateTime = std::format("{:%F %T}", now);
    std::string logLevel = loglevelToText(level);
    std::string text = concat(std::forward<Ts>(ts)...);
    
    std::osyncstream(std::cout) << std::format("{} {} {} {}", dateTime, _module, logLevel, text) << std::endl;
}
