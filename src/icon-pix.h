#pragma once

#include <gdkmm/pixbuf.h>
#include <iostream>
#include <format>
#include <cmath>

class IconPix
{
public:
    IconPix(std::string const & filename);
    
    Glib::RefPtr<Gdk::Pixbuf> active();
    Glib::RefPtr<Gdk::Pixbuf> grayscale();
    Glib::RefPtr<Gdk::Pixbuf> working(int degree);
    
private:
    unsigned char * pixel(Glib::RefPtr<Gdk::Pixbuf> pix, unsigned int x, unsigned int y);
    
    Glib::RefPtr<Gdk::Pixbuf> _pix;
};
