#pragma once

#include <evpn-types.h>

#include <list>
#include <optional>
#include <string>
#include <map>
#include <cstdint>

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
    std::string getText(std::string const & shortCode) const;

    void increaseStat(std::string const & shortCode);

    std::map<std::string, uint64_t> const stat() const;
private:
    
    Status _status;
    std::optional<std::string> _currentLocation;

    std::list<Location> _topLocations;
    std::list<Location> _locations;

    std::map<std::string, uint64_t> _stat;
};
