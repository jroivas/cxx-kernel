#ifndef VESA_H
#define VESA_H

#include "fb.h"

#if 0
struct vbe_t {
        struct vbe_info_block *info;
        struct vbe_mode_info_block *mode;
        char *win;
        int win_low, win_high;
};
#endif

class Vesa : public FB
{
public:
	Vesa();

	FB::ModeConfig *query(FB::ModeConfig *prefer);
	virtual bool configure(ModeConfig *mode);
	void blit();
private:
	//bool lrmi_ok;
	//vbe_t vbe;
};

#endif
