#include <view.h>
#include <iostream>

// How to pass a callable to the GLIB loop: Glib::signal_idle().connect_once([this] () { updateMenu(); });


View::View(boost::asio::io_context & ctx, Model & model):
    _ctx{ctx},
    _model{model},
    _app{Gtk::Application::create("org.bb.expressvpnapplet")}
{
    Glib::signal_idle().connect_once(std::bind(&View::onStartup, this));
}


void View::start()
{
    _app->hold(); // Increase the counter in order not to exit due to lack of windows.
    boost::asio::post(_ctx, [this] () { _app->run(); });
}


void View::stop()
{
    _app->release();
    quitSignal();
}


void View::onStartup()
{
    if (_app->is_remote())
    {
        std::cerr << "Another instance is already running!" << std::endl;
        stop();
        return;
    }

    _icon = Gtk::StatusIcon::create("weather-clear-night");
    _icon->set_tooltip_text("ExpressVPN");
    _icon->signal_activate().connect(std::bind(&View::onIconLeftClick, this));
    _icon->signal_popup_menu().connect(std::bind(&View::onIconRightClick, this, std::placeholders::_1, std::placeholders::_2));

    updateMenu();
}


void View::updateMenu()
{
    for (auto item: _menu.get_children())
    {
        _menu.remove(*item);
    }
    Gtk::MenuItem * quitBtn = Gtk::manage(new Gtk::MenuItem("Quit"));
    quitBtn->signal_activate().connect(std::bind(&View::onQuitBtnPressed, this));
    _menu.append(*quitBtn);
    
    _menu.show_all();
}

void View::onIconLeftClick()
{
    onIconRightClick(GDK_BUTTON_SECONDARY, gtk_get_current_event_time());
}


void View::onIconRightClick(guint mouseButton, guint32 activateTime)
{
    _menu.popup(mouseButton, activateTime);
}


void View::onQuitBtnPressed()
{
    std::cout << "Quit pressed" << std::endl;
    stop();
}


