#pragma once

#include <string>

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


