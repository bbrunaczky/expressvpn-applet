#pragma once

#include <list>
#include <optional>
#include <string>

struct Location
{
    std::string shortCode;
    std::string text;
    bool preferred;
};

enum class Status
{
    INITIALIZING,
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};


class Model
{
public:
    Model();

    void reset();
    void addLocation(Location && location);

    void setStatus(Status status);
    Status status();
    
private:
    
    Status _status;
    std::optional<std::string> _currentLocation;

    std::list<Location> _locations;
};
