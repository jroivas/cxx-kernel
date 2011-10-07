#include "types.h"
#include "kernel.h"

Kernel::Kernel()
{
	video = new Video();
}

Kernel::~Kernel()
{
	delete video;
	video = NULL;
}

int Kernel::run()
{

	if (video!=NULL) {
		video->clear();
		video->print("Hello world!\n");
		video->print("\nC++ kernel.\n");
		video->print("And a TAB\t test!\n");
		video->print("Removing letter A\bB and continuing.\n");
		video->print("\b\b\bABCDEFG\n");

		delete video;
	}


	return 0;
}
