#include "video.h"
#include "string.h"
//#include "port.h"
#include "mm.h"
#include "arch/platform.h"
#include <stdarg.h>

//#define VIDEO_MEMORY_LOCATION 0xB8000
//#define VIDEO_MEMORY_LOCATION 0xC00B8000
#define VIDEO_COLOR_MASK 0x0700
#define CHAR_LF 0x0A
#define CHAR_CR 0x0D
#define CHAR_EMPTY ' '
#define TAB_SIZE 8
#define SCROLL_SIZE 1
//#define PORT 0x3D4

static Video *__global_video = NULL;

Video *Video::get()
{
	if (__global_video==NULL) {
		//__global_video = new Video();
		__global_video = Platform::video();
	}
	return __global_video;
}

Video::Video()
{
	m_x = 0;
	m_y = 0;

	m_width = 80;
	m_height = 25;

	m_videomem = NULL;
	//m_videomem = (unsigned short *)VIDEO_MEMORY_LOCATION;
}

Video::~Video()
{
	m_x = 0;
	m_y = 0;
}

unsigned int Video::size()
{
	return width()*height();
}

unsigned int Video::width()
{
	return m_width;
}

unsigned int Video::height()
{
	return m_height;
}

void Video::resize(int width, int height)
{
	m_width = width;
	m_height = height;
}

void Video::clear()
{
	if (m_videomem==NULL) return;
	unsigned int i = 0;

	while (i<size()) {
		m_videomem[i++] = CHAR_EMPTY | VIDEO_COLOR_MASK;
	}

	m_x = 0;
	m_y = 0;
}

void Video::setCursor()
{
	// Dummy function
#if 0
	unsigned int pos = m_y*width() + m_x;

	Port::out(PORT, 0x0F);
	Port::out(PORT+1, pos&0xFF);
	Port::out(PORT, 0x0E);
	Port::out(PORT+1, (pos>>8)&0xFF);
#endif
}

void Video::scroll()
{
	if (m_videomem==NULL) return;
	if (m_y>=height()) {
		unsigned int size = (height()-1)*width();
		Mem::copy(m_videomem, m_videomem + SCROLL_SIZE*width(), size);
		Mem::setw(m_videomem + size, ' '|VIDEO_COLOR_MASK, width());
		m_y = height()-1;
	}
}

void Video::print(const char *cp)
{
	const char *str = cp;
	const char *ch;

	for (ch = str; *ch; ch++) putCh(*ch);
#if 0
	for (ch = str; *ch; ch++) {
		if (*ch=='\r') {
			m_x = 0;
		}
		else if (*ch=='\n') {
			m_x = 0;
			m_y++;
		}
		else if (*ch=='\b') {
			if (m_x==0 && m_y>0) {
				m_y--;
				m_x = width()-1;
			} else if (m_y==0 && m_x==0) {
				//Do nothing
			} else {
				m_x--;
			}
		}
		else if (*ch=='\t') {
			//for (int i=0; i<TAB_SIZE; i++) putCh(' ');
			m_x += TAB_SIZE;
		} else {
			putCh(*ch);
		}
	}
#endif
}

void Video::print_l(long val, int radix)
{
        if (val==0) {
                putCh('0');
                return;
        }
        bool n = false;
        if (val<0) {
                val *= -1;
                n = true;
        }
        unsigned int l = 0;
        int tmp = val;
        while (tmp>0) { tmp/=radix; l++; }
        if (n) l++;
        if (l==0) l=1;

        char s[256];
        //char *s = (char*)calloc(1,l+1);
        if (n) s[0]='-';
        s[l]=0;
        l--;
        while (l>0) {
                char t = val%radix;
                if (t<10) s[l]='0'+t;
                else s[l]='A'+t-10;
                val/=radix;
                l--;
        } 
        if (val>0) {
                //s[l]='0'+val%radix;
                char t = val%radix;
                if (t<10) s[l]='0'+t;
                else s[l]='A'+t-10;
        }
        print(s);
        //free(s);
}

void Video::print_ul(unsigned long val, int radix)
{
        if (val==0) {
                putCh('0');
                return;
        }
        int l = 0;
        int tmp = val;
        while (tmp>0) { tmp/=radix; l++; }
        if (l==0) l=1;

        char s[256];
        //char *s = (char*)calloc(1,l+1);
        s[l]=0;
        l--;
        while (l>0) {
                char t = val%radix;
                if (t<10) s[l]='0'+t;
                else s[l]='A'+(t-10);
                val/=radix;
                l--;
        } 
        if (val>0) {
                char t = val%radix;
                if (t<10) s[l]='0'+t;
                else s[l]='A'+t-10;
        }
        print(s);
        //free(s);
}

void Video::printf(const char *fmt, ...)
{
#if 0
        Args args;
        args.init(&fmt);
#endif
        va_list al;
        va_start(al, fmt);

	const char *str = fmt;
	const char *ch;
        bool f = false;
        bool s_signed = true;
        int l_cnt = 0;

	for (ch = str; *ch; ch++) {
                if (f) {
                        if (*ch=='%') {
                                putCh('%');
                                f = false;
                        }
                        else if (*ch=='s') {
                                print(va_arg(al, const char*));
                                f = false;
                        }
                        else if (*ch=='l') {
                                //s_signed = false;
                                l_cnt++;
                        }
                        else if (*ch=='x') {
                                putCh('0');
                                putCh('x');
                                print_ul(va_arg(al, unsigned int), 16);
                                f = false;
                        }
                        else if (*ch=='d') {
                                if (!s_signed) {
                                        print_ul(va_arg(al, unsigned int));
                                } else {
                                        print_l(va_arg(al, int));
                                }
                                f = false;
                        } else if (*ch=='u') {
                                if (l_cnt==0) print_ul(va_arg(al, unsigned int));
                                else if (l_cnt==1) print_ul(va_arg(al, unsigned long));
                                else if (l_cnt==2) print_ul(va_arg(al, unsigned long long));
#if 0
                                s_signed = false;
                                if (!s_signed) {
                                        print_ul(va_arg(al, unsigned long));
                                } else {
                                        print_ul(va_arg(al, long));
                                }
#endif
                                f = false;
                        }
                } 
                else if (*ch=='%') {
                        f = true;
                        s_signed = true;
                }
		else if (*ch=='\r') {
			m_x = 0;
		}
		else if (*ch=='\n') {
			m_x = 0;
			m_y++;
		}
		else if (*ch=='\b') {
			if (m_x==0 && m_y>0) {
				m_y--;
				m_x = width()-1;
			} else if (m_y==0 && m_x==0) {
				//Do nothing
			} else {
				m_x--;
			}
		}
		else if (*ch=='\t') {
			m_x += TAB_SIZE;
		} else {
			putCh(*ch);
		}
        }
        //args.end();
}

void Video::putCh(char c)
{
	if (m_videomem==NULL) return;
	if (m_x>=width()) {
		m_x -= width();
		m_y++;
	}

	unsigned int offset = m_y*width() + m_x; 
	if (offset>=size()) {
		scroll();
	}

	m_videomem[offset] = c | VIDEO_COLOR_MASK;
	m_x++;

	scroll();
	setCursor();

#if 0
	//volatile unsigned int a;
	unsigned int a;
	for (a=0; a<0x1ffffff; a++) { }
#endif
}
