#include <config.h>
#include <view.h>

#include <iostream>
#include <format>
#include <algorithm>


View::View(boost::asio::io_context & ctx, Model & model):
    _ctx{ctx},
    _model{model},
    _app{Gtk::Application::create("org.bb.expressvpnapplet")},
    _iconPix(Config::instance().iconFile()),
    _menu{Gtk::manage(new Gtk::Menu)},
    _animateTimer{_ctx}
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


void View::update()
{
    Glib::signal_idle().connect_once(std::bind(&View::updateMenu, this));
}


void View::onStartup()
{
    if (_app->is_remote())
    {
        std::cerr << "Another instance is already running!" << std::endl;
        stop();
        return;
    }

    _icon = Gtk::StatusIcon::create(_iconPix.grayscale());
    _icon->set_tooltip_text("ExpressVPN");
    _icon->signal_activate().connect(std::bind(&View::onIconLeftClick, this));
    _icon->signal_popup_menu().connect(std::bind(&View::onIconRightClick, this, std::placeholders::_1, std::placeholders::_2));

    std::string cssData = ".StatusText {color: #000000; font-weight: bold}";
    auto css = Gtk::CssProvider::create();
    if (not css->load_from_data(cssData))
    {
        throw std::runtime_error("Failed to load CSS");
    }

    auto style_context = Gtk::StyleContext::create();
    auto screen = Gdk::Screen::get_default();
    style_context->add_provider_for_screen(screen, css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    updateMenu();
    animate();
}


void View::updateMenu()
{
    // If I use the same Menu object over time, removing and adding new MenuItems will mess up the things: for some reason the menu gets popped up at the same position as previously.
    // I guess it's related to the 'Connected: <location>' text and with the variance of length of location
    // todo /bb/ try to increase the length of the label.
    _menu = Gtk::manage(new Gtk::Menu);

    updatePix();
    updateMenuStatusSection();
    updateMenuConnectToSection();
    updateMenuFrequentlyUsedSection();
    updateMenuQuitSection();
    _menu->show_all();
}


void View::updatePix()
{
    static Status prevStatus = Status::INITIALIZING;
    if (_model.status() != prevStatus)
    {
        switch (_model.status())
        {
            case Status::DISCONNECTED:
                _animateTimer.cancel();
                _icon->set(_iconPix.grayscale());
                break;
            case Status::CONNECTED:
                _animateTimer.cancel();
                _icon->set(_iconPix.active());
                break;
            case Status::CONNECTING:
                animate();
                break;
                _icon->set(_iconPix.active());
                break;
            default:
                break;
        }
        prevStatus = _model.status();
    }
}


void View::animate(int degree)
{
    _icon->set(_iconPix.working(degree % 360));
    _animateTimer.expires_from_now(boost::posix_time::milliseconds(ANIMATE_INTERVAL));
    _animateTimer.async_wait([this, degree] (boost::system::error_code const & ec)
                             {
                                 if (boost::asio::error::operation_aborted == ec)
                                 {
                                     return;
                                 }
                                 Glib::signal_idle().connect_once(std::bind(&View::animate, this, degree + ANIMATE_ROTATE_STEP));
                             });
}


void View::updateMenuStatusSection()
{
    Gtk::SeparatorMenuItem * separator = Gtk::manage(new Gtk::SeparatorMenuItem());
    separator->set_label("Status");
    _menu->append(*separator);

    Gtk::MenuItem * statusText = Gtk::manage(new Gtk::MenuItem());
    _menu->append(*statusText);
    statusText->set_sensitive(false);
    statusText->get_style_context()->add_class("StatusText");

    
    switch (_model.status())
    {
        case Status::INITIALIZING:
            statusText->set_label("Initializing...");
            break;
        case Status::DISCONNECTED:
            statusText->set_label("Disconnected");
            break;
        case Status::CONNECTING:
        {
            statusText->set_label("Connecting...");
            Gtk::MenuItem * disconnectMenuItem{Gtk::manage(new Gtk::MenuItem("Cancel"))};
            _menu->append(*disconnectMenuItem);
            disconnectMenuItem->signal_activate().connect(std::bind(&View::onDisconnectBtnPressed, this));
        }
        break;
        case Status::CONNECTED:
        {
            std::string text = std::format("Connected to {}", _model.currentLocation()->text);
            statusText->set_label(text);
            Gtk::MenuItem * disconnectMenuItem{Gtk::manage(new Gtk::MenuItem("Disconnect"))};
            _menu->append(*disconnectMenuItem);
            disconnectMenuItem->signal_activate().connect(std::bind(&View::onDisconnectBtnPressed, this));
        }
        break;
        default:
            throw std::runtime_error("We are not supposed to be here...");
    }
}


void View::updateMenuConnectToSection()
{
    if (Status::INITIALIZING == _model.status())
    {
        return;
    }

    Gtk::SeparatorMenuItem * separator = Gtk::manage(new Gtk::SeparatorMenuItem());
    separator->set_label("Connect to...");
    _menu->append(*separator);

    for (auto const & item: _model.topLocations())
    {
        Gtk::MenuItem * button = Gtk::manage(new Gtk::MenuItem(item.text));
        button->signal_activate().connect(std::bind(&View::onConnectBtnPressed, this, item.shortCode));
        _menu->append(*button);
    }

    struct AlphabetMenu
    {
        char start;
        char end;
        Gtk::MenuItem * menuItem;
        Gtk::Menu * subMenu;
    };

    std::vector<AlphabetMenu> aMenus =
        {
            { .start = 'A', .end = 'C', .menuItem = Gtk::manage(new Gtk::MenuItem("A-C")), .subMenu = Gtk::manage(new Gtk::Menu()) },
            { .start = 'D', .end = 'F', .menuItem = Gtk::manage(new Gtk::MenuItem("D-F")), .subMenu = Gtk::manage(new Gtk::Menu()) },
            { .start = 'G', .end = 'I', .menuItem = Gtk::manage(new Gtk::MenuItem("G-I")), .subMenu = Gtk::manage(new Gtk::Menu()) },
            { .start = 'J', .end = 'L', .menuItem = Gtk::manage(new Gtk::MenuItem("J-L")), .subMenu = Gtk::manage(new Gtk::Menu()) },
            { .start = 'M', .end = 'O', .menuItem = Gtk::manage(new Gtk::MenuItem("M-O")), .subMenu = Gtk::manage(new Gtk::Menu()) },
            { .start = 'P', .end = 'R', .menuItem = Gtk::manage(new Gtk::MenuItem("P-R")), .subMenu = Gtk::manage(new Gtk::Menu()) },
            { .start = 'S', .end = 'U', .menuItem = Gtk::manage(new Gtk::MenuItem("S-U")), .subMenu = Gtk::manage(new Gtk::Menu()) },
            { .start = 'V', .end = 'Z', .menuItem = Gtk::manage(new Gtk::MenuItem("V-Z")), .subMenu = Gtk::manage(new Gtk::Menu()) }
        };

    for (auto const & item: aMenus)
    {
        _menu->append(*item.menuItem);
        item.menuItem->set_submenu(*item.subMenu);
    }
    
    struct LocationsPerCountry
    {
        Gtk::Menu * alphabetSubMenu;
        std::map<std::string, std::string> preferred; // text, shortCode
        std::map<std::string, std::string> other; // text, shortCode
    };

    std::map<std::string, LocationsPerCountry> allLocations;
    
    for (auto const & item: _model.locations())
    {
        if (item.country.empty())
        {
            std::cerr << "Ignoring location as it has no country: " << item.text << std::endl;
            continue;
        }
        char startChar = item.country[0];
        if ('A' > startChar || 'Z' < startChar)
        {
            std::cerr << "Ignoring location as its country doesn't start with [A-Z] character" << item.text << std::endl;
            continue;
        }

        allLocations[item.country].alphabetSubMenu = std::find_if(aMenus.begin(),
                                                                  aMenus.end(),
                                                                  [&startChar] (AlphabetMenu const & amenu)
                                                                  {
                                                                      return amenu.start <= startChar && startChar <= amenu.end;
                                                                  })->subMenu;
        if (item.preferred)
        {
            allLocations[item.country].preferred[item.text] = item.shortCode; 
        }
        else
        {
            allLocations[item.country].other[item.text] = item.shortCode; 
        }
    }

    for (auto & [country, locationsPerCountry]: allLocations)
    {
        bool const twoKind = !locationsPerCountry.preferred.empty() && !locationsPerCountry.other.empty();
        bool const multiple = locationsPerCountry.preferred.size() + locationsPerCountry.other.size() > 1;

        Gtk::Menu * alphabetSubMenu = locationsPerCountry.alphabetSubMenu;

        Gtk::MenuItem * countryMenuItem = Gtk::manage(new Gtk::MenuItem(country));
        alphabetSubMenu->append(*countryMenuItem);
        
        if (!multiple)
        {
            if (locationsPerCountry.preferred.empty())
            {
                locationsPerCountry.preferred = std::move(locationsPerCountry.other);
            }
            if (locationsPerCountry.preferred.empty())
            {
                throw std::runtime_error("We are not supposed to be here!");
            }
            std::string const & shortCode = locationsPerCountry.preferred.begin()->second;
            countryMenuItem->signal_activate().connect(std::bind(&View::onConnectBtnPressed, this, shortCode));
            continue;
        }

        // Multiple locations exist in the given country
        Gtk::Menu * countryMenu = Gtk::manage(new Gtk::Menu());
        countryMenuItem->set_submenu(*countryMenu);
        
        auto addLocations = [this, &countryMenu] (std::map<std::string, std::string> const & locations)
                            {
                                for (auto const & [text, shortCode]: locations)
                                {
                                    Gtk::MenuItem * locationMenuItem = Gtk::manage(new Gtk::MenuItem(text));
                                    countryMenu->append(*locationMenuItem);
                                    locationMenuItem->signal_activate().connect(std::bind(&View::onConnectBtnPressed, this, shortCode));
                                }
                            };
        if (twoKind)
        {
            Gtk::SeparatorMenuItem * separator = Gtk::manage(new Gtk::SeparatorMenuItem());
            separator->set_label("Preferred");
            countryMenu->append(*separator);
        }

        addLocations(locationsPerCountry.preferred);

        if (twoKind)
        {
            Gtk::SeparatorMenuItem * separator = Gtk::manage(new Gtk::SeparatorMenuItem());
            separator->set_label("Other");
            countryMenu->append(*separator);
        }

        addLocations(locationsPerCountry.other);
    }

    for (auto const & item: aMenus)
    {
        if (0 == item.subMenu->get_children().size())
        {
            Gtk::MenuItem * noCountryMenuItem = Gtk::manage(new Gtk::MenuItem("(no country)"));
            noCountryMenuItem->set_sensitive(false);
            item.subMenu->append(*noCountryMenuItem);
        }
    }

}


void View::updateMenuFrequentlyUsedSection()
{
    if (0 == _model.stat().size())
    {
        return;
    }

    Gtk::SeparatorMenuItem * separator = Gtk::manage(new Gtk::SeparatorMenuItem());
    separator->set_label("Frequently used");
    _menu->append(*separator);

    std::vector<std::pair<std::string, uint64_t>> freqUsed;
    freqUsed.reserve(_model.stat().size());
    for (auto const & item: _model.stat())
    {
        freqUsed.push_back(item);
    }
    std::sort(freqUsed.begin(),
              freqUsed.end(),
              [this] (std::pair<std::string, uint64_t> const & lhs, std::pair<std::string, uint64_t> const & rhs)
              {
                  if (lhs.second > rhs.second)
                  {
                      return true;
                  }
                  else if (lhs.second == rhs.second && _model.getText(lhs.first) < _model.getText(rhs.first))
                  {
                      return true;
                  }
                  return false;
              });
    for (auto i=0u; i < std::min(freqUsed.size(), static_cast<uint64_t>(Config::instance().frequentlyUsed())); ++i)
    {
        Gtk::MenuItem * locationMenuItem = Gtk::manage(new Gtk::MenuItem(_model.getText(freqUsed[i].first)));
        _menu->append(*locationMenuItem);
        locationMenuItem->signal_activate().connect(std::bind(&View::onConnectBtnPressed, this, freqUsed[i].first));
    }

}


void View::updateMenuQuitSection()
{
    _menu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
        
    Gtk::MenuItem * quitBtn = Gtk::manage(new Gtk::MenuItem("Quit"));
    quitBtn->signal_activate().connect(std::bind(&View::onQuitBtnPressed, this));
    _menu->append(*quitBtn);
}


void View::onIconLeftClick()
{
    onIconRightClick(GDK_BUTTON_SECONDARY, gtk_get_current_event_time());
}


void View::onIconRightClick(guint mouseButton, guint32 activateTime)
{
    _menu->popup(mouseButton, activateTime);

}


void View::onQuitBtnPressed()
{
    stop();
}


void View::onConnectBtnPressed(std::string const & shortCode)
{
    connectSignal(shortCode);
}


void View::onDisconnectBtnPressed()
{
    disconnectSignal();
}
