#include "mm.h"
#include "string.hh"
#include "x11.h"
#include "platform.h"

X11::X11()
{
}

X11::~X11()
{
}

FB::ModeConfig *X11::query(FB::ModeConfig *prefer)
{
    FB::ModeConfig *conf = (FB::ModeConfig*)MM::instance()->alloc(sizeof(FB::ModeConfig));
    conf->width = prefer->width;
    conf->height = prefer->height;
    conf->depth = 32; //FIXME
    conf->bytes_per_line = conf->width * conf->depth / 8;
    return conf;
}

bool X11::configure(ModeConfig *mode)
{
    if (mode==NULL) return false;

    m_disp = XOpenDisplay(NULL);
    if (m_disp == NULL) {
        return false;
    }

    int screen = DefaultScreen(m_disp);
    unsigned long black = BlackPixel(m_disp, screen);
    unsigned long white = WhitePixel(m_disp, screen);

    m_visual = DefaultVisual(m_disp,screen);
    m_win = XCreateSimpleWindow(
            m_disp,
            DefaultRootWindow(m_disp),
            0, 0,
            mode->width, mode->height,
            0,
            white, black);
    m_pix = XCreatePixmap(
        m_disp,
        m_win,
        mode->width,
        mode->height,
        (mode->depth < 32) ? mode->depth : 24);

    m_gc = XCreateGC(m_disp, m_pix, 0, NULL);
    XSetForeground(m_disp, m_gc, white);

    XMapWindow(m_disp, m_win);
    XSync(m_disp, False);

    return FB::configure(mode);
}

void X11::clear()
{
}

void X11::blit()
{
    XImage *img = XGetImage(
            m_disp, m_pix,
            0, 0,
            m_current->width,
            m_current->height,
            AllPlanes,
            ZPixmap);

    Mem::copy(
        img->data,
        m_buffer,
        img->bytes_per_line * img->height);

    XPutImage(
        m_disp, m_pix,
        m_gc, img,
        0, 0, 0, 0,
        m_current->width,
        m_current->height);
    XCopyArea(m_disp,
        m_pix,
        m_win,
        m_gc,
        0, 0,
        m_current->width,
        m_current->height,
        0, 0);

    XFree(img);
    XFlush(m_disp);
}
