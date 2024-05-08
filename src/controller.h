#pragma once

#include <model.h>
#include <view.h>

#include <boost/asio.hpp>

class Controller
{
public:
    Controller(boost::asio::io_context & ctx);
//    void refreshLocations();

private:

    void quitCallback();
    
    boost::asio::io_context & _ctx;
    
    Model _model;
    View _view;
    
};
