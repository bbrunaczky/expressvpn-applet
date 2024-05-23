#pragma once

#include <model.h>
#include <icon-pix.h>

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <gtkmm.h>

class View
{
public:
    View(boost::asio::io_context & ctx, Model & model);

    boost::signals2::signal<void ()> quitSignal;
    boost::signals2::signal<void (std::string const &)> connectSignal;
    boost::signals2::signal<void ()> disconnectSignal;

    void start();
    void stop();

    void update();
    
private:
    void onStartup();

    void updatePix();
    void updateMenu();
    void updateMenuStatusSection();
    void updateMenuConnectToSection();
    void updateMenuFrequentlyUsedSection();
    void updateMenuQuitSection();
    
    void onIconLeftClick();
    void onIconRightClick(guint button, guint32 activateTime);
    
    void onQuitBtnPressed();
    void onConnectBtnPressed(std::string const & shortCode);
    void onDisconnectBtnPressed();

    void animate(int degree = ANIMATE_ROTATE_STEP);
    
    boost::asio::io_context & _ctx;
    Model & _model;

    Glib::RefPtr<Gtk::Application> _app;
    IconPix _iconPix;
    Glib::RefPtr<Gtk::StatusIcon> _icon;
    
    Gtk::Menu * _menu;
    boost::asio::deadline_timer _animateTimer;
    constexpr static unsigned int const ANIMATE_INTERVAL{20};
    constexpr static int const ANIMATE_ROTATE_STEP{-20};
    

};
