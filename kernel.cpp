#include "video.h"
#include "cxa.h"
#include "paging.h"
#include "gdt.h"

#if 0
Video video;

extern "C" int main()
{
	video.clear();
	video.print("Hello world!\n");
	video.print("\nC++ kernel.\n");

	while(1) { }

	return 0;
}
#else

extern "C" int main()
{
	Video *video = new Video();
	
	video->clear();
	video->print("Hello world!\n");
	video->print("\nC++ kernel.\n");

	delete video;

	return 0;
}

#endif
