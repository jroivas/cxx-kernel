#ifndef VIDEO_X86_H
#define VIDEO_X86_H

#include "video.h"

class VideoX86 : public Video
{
public:
	VideoX86();
	virtual void putCh(char c);

protected:
	void setCursor();
};

#endif
