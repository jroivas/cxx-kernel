#ifndef X11_H
#define X11_H

#include <X11/Xlib.h>
#include "fb.h"

class X11 : public FB
{
public:
	X11();
	~X11();

	FB::ModeConfig *query(FB::ModeConfig *prefer);
	virtual bool configure(ModeConfig *mode);
	virtual void clear();
	void blit();

private:
	Display *m_disp;
	Window m_win;
	Pixmap m_pix;
	Visual *m_visual;
	GC m_gc;
};

#endif
