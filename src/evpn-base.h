#pragma once

#include <evpn-types.h>
#include <functional>
#include <optional>
#include <boost/signals2.hpp>
#include <boost/asio.hpp>

class EvpnBase
{
public:
    virtual ~EvpnBase() = default;

    boost::asio::io_context & context();
    
    // Periodic status update
    boost::signals2::signal<void (Status, std::optional<std::string>)> statusSignal;
    void periodicStatusQuery(); // Starts the periodic query
    
    virtual void getLocations(std::function<void (std::list<Location>)>) = 0;
    virtual void connect(std::string const & shortCode, std::function<void ()> callback) = 0;
    virtual void disconnect(std::function<void ()> callback) = 0;
    virtual void cancel() = 0;
    
protected:
    EvpnBase(boost::asio::io_context & ctx, uint32_t statusUpdateInterval = 0); // Seconds. 0 means no periodic update

    virtual void getStatus(std::function<void (Status, std::optional<std::string>)>) = 0;

private:

    boost::asio::io_context & _ctx;
    uint32_t _statusUpdateInterval;
    boost::asio::deadline_timer _statusUpdateTimer;

};
