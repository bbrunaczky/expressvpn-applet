#include <controller.h>
#include <evpn-process.h>
#include <iostream>


Controller::Controller(boost::asio::io_context & ctx):
    Logger("controller"),
    _ctx{ctx},
    _view{_ctx, _model},
    _evpn{new EvpnProcess(_ctx, 10)}
{
    _view.quitSignal.connect(std::bind(&Controller::quitCallback, this));
    _view.connectSignal.connect(std::bind(&Controller::connectCallback, this, std::placeholders::_1));
    _view.disconnectSignal.connect(std::bind(&Controller::disconnectCallback, this));
    _view.start();

    _model.setStatus(Status::INITIALIZING);
    _view.update();

    _evpn->statusSignal.connect(std::bind(&Controller::periodicStatusUpdateCallback, this, std::placeholders::_1, std::placeholders::_2));

    _evpn->getLocations(std::bind(&Controller::updateLocations, this, std::placeholders::_1));
}


void Controller::quitCallback()
{
    debug("Stopping event loop");
    _ctx.stop();
}


void Controller::connectCallback(std::string const & shortCode)
{
    info("Connect request arrived. Location:", _model.getText(shortCode), std::format("({})", shortCode));
    _model.setCurrentLocation(shortCode);

    auto connect = [this, shortCode] ()
                   {
                       info("Connecting to", _model.getText(shortCode), std::format("({})", shortCode));
                       _model.setStatus(Status::CONNECTING);
                       _view.update();
                       _evpn->connect(shortCode,
                                      [this, shortCode] ()
                                      {
                                          info("Connected to", _model.getText(shortCode), std::format("({})", shortCode));
                                          _model.setStatus(Status::CONNECTED);
                                          _view.update();
                                      });
                   };
    if (_model.status() != Status::DISCONNECTED)
    {
        info("Not in DISCONNECTED state, disconnect first");
        _evpn->disconnect([this, shortCode, connect] ()
                          {
                              info("Disconnected");
                              _model.setStatus(Status::DISCONNECTED);
                              connect();
                          });
    }
    else
    {
        connect();
    }

    _model.increaseStat(shortCode);
}



void Controller::disconnectCallback()
{
    info("Disconnect request arrived");

    _evpn->disconnect([this] ()
                      {
                          _model.setStatus(Status::DISCONNECTED);
                          _view.update();
                          info("Disconnected");
                      });
    
}


void Controller::updateLocations(std::list<Location> locations)
{
    debug("Updating locations");
    for (auto & l: locations)
    {
        if (l.shortCode == "smart")
        {
            l.text = "Smart: " + l.text;
        }
        if (l.shortCode == "xv" || l.shortCode == "smart")
        {
            debug("Adding TOP location:", std::format("\"{}\" ({}), country: {}, preferred: {}", l.text, l.shortCode, l.country, l.preferred));
            _model.addTopLocation(l);
        }
        else
        {
            debug("Adding location:", std::format("\"{}\" ({}), country: {}, preferred: {}", l.text, l.shortCode, l.country, l.preferred));
            _model.addLocation(l);
        }
    }    
    // View-update is not needed here as in init state we don't show anything.
    // Start querying the status
    _evpn->periodicStatusQuery();
}


void Controller::periodicStatusUpdateCallback(Status status, std::optional<std::string> locationText)
{
    if (_model.status() != status)
    {
        if (Status::DISCONNECTED == status && Status::CONNECTING == _model.status())
        {
            debug("Ignoring external status change as we are connecting now");
            return;
        }
        info("expressvpn shows new status:", statusToText(status));
        _model.setStatus(status);
        if (Status::CONNECTED == status)
        {
            if (!locationText)
            {
                throw std::runtime_error("Empty locationText at Controller::periodicStatusUpdateCallback()!");
            }
            std::string shortCode = _model.getShortCode(*locationText);
            if (shortCode.empty())
            {
                throw std::runtime_error("Empty shortCode at Controller::periodicStatusUpdateCallback()!");
            }
            _model.setCurrentLocation(shortCode);
        }
        _view.update();
    }
}
