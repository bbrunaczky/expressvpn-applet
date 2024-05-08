#include <model.h>

Model::Model()
{
    reset();
}


void Model::reset()
{
    _locations.clear();
    _status = Status::INITIALIZING;
    _currentLocation.reset();
}


void Model::addLocation(Location && location)
{
    _locations.push_back(std::move(location));
}


void Model::setStatus(Status status)
{
    _status = status;
}


Status Model::status()
{
    return _status;
}


