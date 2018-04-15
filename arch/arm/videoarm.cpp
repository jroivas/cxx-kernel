#include "videoarm.h"

volatile unsigned int * const UART0DR = (unsigned int *)0x101f1000;

VideoARM::VideoARM() : Video()
{
	m_videomem = nullptr;
}

void VideoARM::setCursor()
{
}

#if 0
void print_uart0(const char *s) {
 while(*s != '\0') { /* Loop until end of string */
 *UART0DR = (unsigned int)(*s); /* Transmit char */
 s++; /* Next char */
 }
}
#endif

void VideoARM::putCh(char c)
{
	if (c=='\n') {
		//::printf("\n");
 		*UART0DR = (unsigned int)('\n');
	} else {
		//::printf("%c",c);
 		*UART0DR = (unsigned int)(c);
	}
	Video::putCh(c);
}
