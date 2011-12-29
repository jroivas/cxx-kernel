#ifndef VIDEO_LINUX_H
#define VIDEO_LINUX_H

#include "video.h"

class VideoLinux : public Video
{
public:
	VideoLinux();
	virtual void putCh(char c);

protected:
	void setCursor();
};

#endif
