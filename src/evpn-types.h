#pragma once

#include <string>
#include <stdexcept>

struct Location
{
    std::string shortCode;
    std::string text;
    std::string country;
    bool preferred;
};

enum class Status
{
    INITIALIZING,
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};

constexpr std::string statusToText(Status status)
{
    switch (status)
    {
        case Status::INITIALIZING:
            return "INITIALIZING";
            break;
        case Status::DISCONNECTED:
            return "DISCONNECTED";
            break;
        case Status::CONNECTING:
            return "CONNECTING";
            break;
        case Status::CONNECTED:
            return "CONNECTED";
            break;
        default:
            throw std::runtime_error("Invalid Status!");
    };
}


