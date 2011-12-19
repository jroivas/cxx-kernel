#include "videox86.h"
#include "port.h"
#include "timer.h"

#define VIDEO_MEMORY_LOCATION 0xB8000
#define PORT 0x3D4
#define COM1 0x3F8

static bool __uart_initted = false;
static bool __uart_avail = false;

VideoX86::VideoX86() : Video()
{
	m_videomem = (unsigned short *)VIDEO_MEMORY_LOCATION;
}

void uart_init()
{
	Port::out(COM1+2, 0);

	Port::out(COM1+3, 0x80);
	Port::out(COM1+0, 115200/115200);
	Port::out(COM1+1, 0);
	Port::out(COM1+3, 3);
	Port::out(COM1+4, 0);
	Port::out(COM1+1, 1);

	__uart_initted = true;
	if (Port::in(COM1+5)==0xff) {
		__uart_avail = false;
		return;
	}
	__uart_avail = true;
}

void uart_putc(char c)
{
	if (!__uart_initted || !__uart_avail) return;

	for (int i=0; i<128; i++) {
		if ((Port::in(COM1+5) & 0x20)!=0) {
			break;
		}
                Timer::get()->wait(1);
	}
	Port::out(COM1, c);
}

void VideoX86::setCursor()
{
	unsigned int pos = m_y*width() + m_x;

	Port::out(PORT, 0x0F);
	Port::out(PORT+1, pos&0xFF);
	Port::out(PORT, 0x0E);
	Port::out(PORT+1, (pos>>8)&0xFF);
}

void VideoX86::putCh(char c)
{
	if (!__uart_initted) uart_init();
	uart_putc(c);
	if (c=='\n') uart_putc('\r');
	Video::putCh(c);
}
