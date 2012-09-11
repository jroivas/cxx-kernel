#ifndef ARMFB_H
#define ARMFB_H

#include "fb.h"

class ARMFB : public FB
{
public:
	ARMFB();
	~ARMFB();

	FB::ModeConfig *query(FB::ModeConfig *prefer);
	virtual bool configure(ModeConfig *mode);
	virtual void clear();
	void blit();

private:
	//uint16_t *base;
	//void *plio;
};

#endif
