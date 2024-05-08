#include <controller.h>


Controller::Controller(boost::asio::io_context & ctx):
    _ctx{ctx},
    _view{_ctx, _model}
{
    _view.quitSignal.connect(std::bind(&Controller::quitCallback, this));
    _view.start();
}


void Controller::quitCallback()
{
    _ctx.stop();
}
