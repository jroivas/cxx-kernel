#ifndef VIDEO_X86_H
#define VIDEO_X86_H

#include "video.h"

class VideoX86 : public Video
{
public:
	VideoX86();

protected:
	void setCursor();
};

#endif
