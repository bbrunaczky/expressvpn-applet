#pragma once

#include <evpn-types.h>

#include <list>
#include <optional>
#include <string>

class Model
{
public:
    Model();

    void reset();

    void setStatus(Status status);
    Status status();

    void addLocation(Location const & location);
    std::list<Location> const & locations();
    
    void addTopLocation(Location const & location);
    std::list<Location> const & topLocations();

    void setCurrentLocation(std::string const & shortCode);
    std::optional<Location> currentLocation();

    std::string getShortCode(std::string const & locationText) const;
private:
    
    Status _status;
    std::optional<std::string> _currentLocation;

    std::list<Location> _topLocations;
    std::list<Location> _locations;
};
