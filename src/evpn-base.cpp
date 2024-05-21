#include <evpn-base.h>
#include <process.h>
#include <iostream>

EvpnBase::EvpnBase(boost::asio::io_context & ctx, uint32_t statusUpdateInterval):
    _ctx{ctx},
    _statusUpdateInterval{statusUpdateInterval},
    _statusUpdateTimer{ctx}
{
}


boost::asio::io_context & EvpnBase::context()
{
    return _ctx;
}


void EvpnBase::periodicStatusQuery()
{
    getStatus([this] (Status status, std::optional<std::string> location)
              {
                  statusSignal(status, location);
              });
        
    if (_statusUpdateInterval == 0)
    {
        return;;
    }
    
    _statusUpdateTimer.expires_from_now(boost::posix_time::seconds(_statusUpdateInterval));
    _statusUpdateTimer.async_wait(
        [this] (boost::system::error_code const & ec)
        {
            if (boost::asio::error::operation_aborted == ec)
            {
                return;
            }
            periodicStatusQuery();
        });
}
