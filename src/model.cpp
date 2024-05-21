#include <model.h>
#include <algorithm>

Model::Model()
{
    reset();
}


void Model::reset()
{
    _locations.clear();
    _status = Status::INITIALIZING;
    _currentLocation.reset();

    // todo /bb/ tests
    // _status = Status::CONNECTED;
    // _currentLocation = "se";
    
    // addTopLocation({ .shortCode = "smart", .text = "Smart Location", .preferred = true });
    // addTopLocation({ .shortCode = "pickforme", .text = "Pick for Me", .preferred = true });

    // addLocation({ .shortCode = "se2", .text = "Sweden - 2", .country = "Sweden (SE)", .preferred = true });
    // addLocation({ .shortCode = "se", .text = "Sweden", .country = "Sweden (SE)", .preferred = true });

    // addLocation({ .shortCode = "uswd", .text = "USA - Washington DC", .country = "United States (US)", .preferred = true });
    // addLocation({ .shortCode = "usny", .text = "USA - New York", .country = "United States (US)", .preferred = true });
    // addLocation({ .shortCode = "ussf", .text = "USA - San Francisco", .country = "United States (US)", .preferred = false });
    // addLocation({ .shortCode = "hu", .text = "Hungary (HU)", .country = "Hungary", .preferred = false });
}


void Model::setStatus(Status status)
{
    _status = status;
}


Status Model::status()
{
    return _status;
}


void Model::addLocation(Location const & location)
{
    _locations.push_back(location);
}


std::list<Location> const & Model::locations()
{
    return _locations;
}


void Model::addTopLocation(Location const & location)
{
    _topLocations.push_back(location);
}


std::list<Location> const & Model::topLocations()
{
    return _topLocations;
}


void Model::setCurrentLocation(std::string const & shortCode)
{
    _currentLocation = shortCode;
}


std::optional<Location> Model::currentLocation()
{
    if (_currentLocation)
    {
        auto it = std::find_if(_topLocations.begin(),
                               _topLocations.end(),
                               [this] (Location const & item)
                               {
                                   return item.shortCode == *_currentLocation;
                               });
        if (_topLocations.end() != it)
        {
            return *it;
        }
        
        it = std::find_if(_locations.begin(),
                          _locations.end(),
                          [this] (Location const & item)
                          {
                              return item.shortCode == *_currentLocation;
                          });
        if (_locations.end() != it)
        {
            return *it;
        }
    }
    return std::optional<Location>();
}


std::string Model::getShortCode(std::string const & locationText) const
{
    std::string shortCode;
    
    for (auto const & list: { _topLocations, _locations} )
    {
        auto it = std::find_if(list.begin(),
                               list.end(),
                               [locationText] (Location const & location)
                               {
                                   return location.text == locationText;
                               });
        if (list.end() != it)
        {
            shortCode = it->shortCode;
        }
    }
    return shortCode;
}

