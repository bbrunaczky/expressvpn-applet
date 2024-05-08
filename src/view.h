#pragma once

#include <model.h>

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <gtkmm.h>

class View
{
public:
    View(boost::asio::io_context & ctx, Model & model);

    boost::signals2::signal<void ()> quitSignal;

    void start();
    void stop();
    
private:
    void onStartup();

    void updateMenu();
    
    void onIconLeftClick();
    void onIconRightClick(guint button, guint32 activateTime);
    
    void onQuitBtnPressed();

    boost::asio::io_context & _ctx;
    Model & _model;

    Glib::RefPtr<Gtk::Application> _app;
    Glib::RefPtr<Gtk::StatusIcon> _icon;
    Gtk::Menu _menu;

};
