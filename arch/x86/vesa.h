#ifndef VESA_H
#define VESA_H

#include "fb.h"
#include "types.h"

class Vesa : public FB
{
public:
	Vesa();

	FB::ModeConfig *query(FB::ModeConfig *prefer);
	virtual bool configure(ModeConfig *mode);
	void blit();

private:
	ptr_t getVESA();
	void setMode(ModeConfig *mode);
	int modeDiff(FB::ModeConfig *conf, FB::ModeConfig *cmp);
	void copyMode(FB::ModeConfig *dest, FB::ModeConfig *src);
};

#endif
