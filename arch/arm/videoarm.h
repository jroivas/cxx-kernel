#ifndef VIDEO_ARM_H
#define VIDEO_ARM_H

#include "video.h"

class VideoARM : public Video
{
public:
	VideoARM();
	virtual void putCh(char c);

protected:
	void setCursor();
};

#endif
