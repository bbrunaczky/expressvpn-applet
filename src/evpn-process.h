#pragma once

#include <evpn-base.h>
#include <process.h>
#include <boost/asio.hpp>


class EvpnProcess: public EvpnBase
{
public:
    EvpnProcess(boost::asio::io_context & ctx, uint32_t statusUpdateInterval = 0);

    void getLocations(std::function<void (std::list<Location>)>) override;
    void getStatus(std::function<void (Status, std::optional<std::string>)> callback) override;
    void connect(std::string const & shortCode, std::function<void ()> callback) override;
    void disconnect(std::function<void ()> callback) override;

    void cancel() override;
    
private:

    boost::asio::io_context & _ctx;

    std::shared_ptr<Process> _connectProcess;
    

};
