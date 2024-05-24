#pragma once

#include <logger.h>
#include <model.h>
#include <view.h>
#include <evpn-base.h>

#include <boost/asio.hpp>

class Controller: public Logger
{
public:
    Controller(boost::asio::io_context & ctx);

private:

    void quitCallback();
    void connectCallback(std::string const & shortCode);
    void disconnectCallback();

    void updateLocations(std::list<Location> locations);
    void periodicStatusUpdateCallback(Status status, std::optional<std::string> locationText);
    
    boost::asio::io_context & _ctx;
    
    Model _model;
    View _view;

    std::unique_ptr<EvpnBase> _evpn;
};
