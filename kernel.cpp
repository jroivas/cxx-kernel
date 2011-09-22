#include "video.h"
#include "cxa.h"

Video video;

extern "C" int main()
{
	video.clear();
	video.print("Hello world!\n");
	video.print("\nC++ kernel.\n");
	return 0;
}
