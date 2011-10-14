#include "fb.h"
#include "types.h"

FB::FB()
{
	current = NULL;
	buffer = NULL;
	backbuffer = NULL;
}

unsigned char *FB::data()
{
	if (buffer == NULL) return NULL;
	return buffer;
}

void FB::swap()
{
	if (buffer == NULL) return;
	if (backbuffer == NULL) return;

	unsigned char *tmp = buffer;
	buffer = backbuffer;
	backbuffer = tmp;

	//blit(); //TODO/XXX do we want this here?
}
