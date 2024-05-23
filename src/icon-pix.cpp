#include <icon-pix.h>

IconPix::IconPix(std::string const & filename)
{
    _pix = Gdk::Pixbuf::create_from_file(filename);
};

    
Glib::RefPtr<Gdk::Pixbuf> IconPix::active()
{
    return _pix;
}

    
Glib::RefPtr<Gdk::Pixbuf> IconPix::grayscale()
{
    Glib::RefPtr<Gdk::Pixbuf> pix = _pix->copy();
    int w = pix->get_width();
    int h = pix->get_height();

    for (int y=0; y<h; ++y)
    {
        for (int x=0; x<w; ++x)
        {
            unsigned char * p = pixel(pix, x, y);
            p[0] = 0xA0;
            p[1] = 0xA0;
            p[2] = 0xA0;
        }
    }
    return pix;
}


Glib::RefPtr<Gdk::Pixbuf> IconPix::working(int degree)
{
    static double const PI{2 * std::acos(0)};
    double rad = (2*PI / 360) * degree;

    int w = _pix->get_width();
    int h = _pix->get_height();
    Glib::RefPtr<Gdk::Pixbuf> newPix = Gdk::Pixbuf::create (Gdk::Colorspace::COLORSPACE_RGB, true, 8, w, h);

    int centerX = w/2;
    int centerY = h/2;
        
    for (int y=0; y<h; ++y)
    {
        for (int x=0; x<w; ++x)
        {
            unsigned char * originalPixel = pixel(_pix, x, y);

            int newX = (x-centerX) * std::cos(rad) - (y-centerY) * std::sin(rad) + centerX;
            int newY = (x-centerX) * std::sin(rad) + (y-centerY) * std::cos(rad) + centerY;

            if (0 <= newX and newX < w && 0 <= newY && newY < h)
            {
                unsigned char * newPixel = pixel(newPix, newX, newY);
                newPixel[0] = 0xA0;
                newPixel[1] = 0xA0;
                newPixel[2] = 0xA0;
                newPixel[3] = originalPixel[3];
            }
        }
    }
    return newPix;
}
    

unsigned char * IconPix::pixel(Glib::RefPtr<Gdk::Pixbuf> pix, unsigned int x, unsigned int y)
{
    unsigned char * pixels = pix->get_pixels();
    int nChannels = pix->get_n_channels();
    int rowstride = pix->get_rowstride();

    unsigned char * p = pixels + y*rowstride + x*nChannels;
    return p;
}
