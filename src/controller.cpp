#include <controller.h>

#include <iostream>


Controller::Controller(boost::asio::io_context & ctx):
    _ctx{ctx},
    _view{_ctx, _model}
{
    _view.quitSignal.connect(std::bind(&Controller::quitCallback, this));
    _view.connectSignal.connect(std::bind(&Controller::connectCallback, this, std::placeholders::_1));
    _view.disconnectSignal.connect(std::bind(&Controller::disconnectCallback, this));
    _view.start();

    // todo /bb/ get it from the app
    _model.setStatus(Status::DISCONNECTED);
    _view.update();
}


void Controller::quitCallback()
{
    _ctx.stop();
}


void Controller::connectCallback(std::string const & shortCode)
{
    std::cout << "Connect requested: " << shortCode << std::endl;
    _model.setStatus(Status::CONNECTED);
    _model.setCurrentLocation(shortCode);
    _view.update();
}


void Controller::disconnectCallback()
{
    std::cout << "Disconnect requested" << std::endl;
    _model.setStatus(Status::DISCONNECTED);
    _view.update();
}
