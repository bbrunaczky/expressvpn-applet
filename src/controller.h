#pragma once

#include <model.h>
#include <view.h>

#include <boost/asio.hpp>

class Controller
{
public:
    Controller(boost::asio::io_context & ctx);

private:

    void quitCallback();
    void connectCallback(std::string const & shortCode);
    void disconnectCallback();
    
    boost::asio::io_context & _ctx;
    
    Model _model;
    View _view;
    
};
