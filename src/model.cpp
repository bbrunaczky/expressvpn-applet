#include <config.h>
#include <model.h>
#include <algorithm>
#include <iostream>
#include <fstream>

Model::Model():
    Logger{"model"}
{
    reset();

    std::fstream inp(Config::instance().statFile(), std::ios::in);
    while (inp)
    {
        std::string shortCode;
        uint64_t value{0};
        inp >> shortCode;
        if (inp.eof())
        {
            break;
        }
        inp >> value;
        _stat[shortCode] = value;
    }
    inp.close();
}


void Model::reset()
{
    _locations.clear();
    _status = Status::INITIALIZING;
    _currentLocation.reset();
}


void Model::setStatus(Status status)
{
    if (_status != status)
    {
        debug("Status changed:", statusToText(_status), "->", statusToText(status));
        _status = status;
    }
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


std::string Model::getText(std::string const & shortCode) const
{
    std::string text;
    
    for (auto const & list: { _topLocations, _locations} )
    {
        auto it = std::find_if(list.begin(),
                               list.end(),
                               [shortCode] (Location const & location)
                               {
                                   return location.shortCode == shortCode;
                               });
        if (list.end() != it)
        {
            text = it->text;
        }
    }
    return text;
}


void Model::increaseStat(std::string const & shortCode)
{
    ++_stat[shortCode];

    std::fstream out(Config::instance().statFile(), std::ios::out | std::ios::trunc);
    for (auto const & [key, value]: _stat)
    {
        out << key << " " << value << std::endl;
    }
    out.close();

}


std::map<std::string, uint64_t> const Model::stat() const
{
    return _stat;
}
