#include "video.h"
#include "cxa.h"
#include "paging.h"
#include "gdt.h"
#include "types.h"

#if 0
//Use this if you don't have a memory management and malloc
Video video;

int main()
{
	video.clear();
	video.print("Hello world!\n");
	video.print("\nC++ kernel.\n");

	while(1) { }

	return 0;
}
#else

//extern "C" int main()
int main()
{
	Video *video = new Video();

	if (video!=NULL) {
		video->clear();
		video->print("Hello world!\n");
		video->print("\nC++ kernel.\n");
		video->print("And a TAB\t test!\n");
		video->print("Removing letter A\bB and continuing.\n");
		video->print("\b\b\bABCDEFG\n");
#if 0
	for (int i=0; i<22; i++) {
		video->print("Line\n");
	}
#endif

		delete video;
	}

	while(1) { }


	return 0;
}

#endif
