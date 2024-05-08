#pragma once

#include <list>
#include <optional>
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


class Model
{
public:
    Model();

    void reset();

    void setStatus(Status status);
    Status status();
    
    void addLocation(Location && location);
    std::list<Location> const & locations();
    
    void addTopLocation(Location && location);
    std::list<Location> const & topLocations();

    std::optional<Location> currentLocation();
private:
    
    Status _status;
    std::optional<std::string> _currentLocation;

    std::list<Location> _locations;
    std::list<Location> _topLocations;
};
