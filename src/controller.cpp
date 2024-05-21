#include <controller.h>
#include <evpn-process.h>
#include <iostream>


Controller::Controller(boost::asio::io_context & ctx):
    _ctx{ctx},
    _view{_ctx, _model},
    _evpn{new EvpnProcess(_ctx, 10)}
{
    // todo /bb/ the signals should be transferred to a non-gtk thread
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
    _ctx.stop();
}


void Controller::connectCallback(std::string const & shortCode)
{
    _model.setStatus(Status::CONNECTING);
    _model.setCurrentLocation(shortCode);
    _view.update();

    auto finalize = [this] ()
                    {
                        _model.setStatus(Status::CONNECTED);
                        _view.update();
                    };
    if (_model.status() != Status::DISCONNECTED)
    {
        _evpn->cancel();
        _evpn->disconnect([this, shortCode, finalize] ()
                          {
                              _evpn->connect(shortCode, finalize);
                          });
    }
    else
    {
        _evpn->connect(shortCode, finalize);
    }
}


void Controller::disconnectCallback()
{
    _model.setStatus(Status::DISCONNECTED);
    _view.update();

    _evpn->disconnect([] () {});
    
}


void Controller::updateLocations(std::list<Location> locations)
{
    for (auto & l: locations)
    {
        if (l.shortCode == "smart")
        {
            l.text = "Smart: " + l.text;
        }
        if (l.shortCode == "xv" || l.shortCode == "smart")
        {
            // std::cout << "Adding TOP location: " << l.shortCode << ", " << l.text << ", " << l.preferred << std::endl;
            _model.addTopLocation(l);
        }
        else
        {
            // std::cout << "Adding location: " << l.shortCode << ", " << l.text << ", " << l.preferred << std::endl;
            _model.addLocation(l);
        }
    }    
    _model.setStatus(Status::DISCONNECTED);
    // View-update is not needed here as in init state we don't show anything.
    // Start querying the status
    _evpn->periodicStatusQuery();
}


void Controller::periodicStatusUpdateCallback(Status status, std::optional<std::string> locationText)
{
    if (_model.status() != status)
    {
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
