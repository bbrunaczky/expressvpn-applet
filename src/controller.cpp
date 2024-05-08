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
}


void Controller::quitCallback()
{
    _ctx.stop();
}


void Controller::connectCallback(std::string const & shortCode)
{
    std::cout << "Connect requested: " << shortCode << std::endl;
}


void Controller::disconnectCallback()
{
    std::cout << "Disconnect requested" << std::endl;
}
