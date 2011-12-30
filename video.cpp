#include "video.h"
#include "string.h"
//#include "port.h"
#include "mm.h"
#include "font.h"
#include "arch/platform.h"
#include <stdarg.h>

#define VIDEO_COLOR_MASK 0x0700
#define CHAR_LF 0x0A
#define CHAR_CR 0x0D
#define CHAR_EMPTY ' '
#define TAB_SIZE 8
#define SCROLL_SIZE 1

static Video *__global_video = NULL;

Video *Video::get()
{
	if (__global_video==NULL) {
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
	m_font = NULL;
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
#if 1
	if (Platform::fb()!=NULL && Platform::fb()->isConfigured()) {
	} else {
		if (m_videomem==NULL) return;
		unsigned int i = 0;
		unsigned int csize = size();

		while (i<csize) {
			m_videomem[i++] = CHAR_EMPTY | VIDEO_COLOR_MASK;
		}
	}
	m_x = 0;
	m_y = 0;
#endif
}

void Video::scroll()
{
#if 1
	if (Platform::fb()!=NULL && Platform::fb()->isConfigured()) {
	} else {
		if (m_videomem==NULL) return;
		if (m_y>=height()) {
			unsigned int ss = (height()-2)*width();
			Mem::copy(m_videomem, m_videomem + SCROLL_SIZE*width()*2, ss*2);
			Mem::setw(m_videomem + ss, 0, width());
			//Mem::setw(m_videomem + ss-width(), ' '|VIDEO_COLOR_MASK, width());
			m_y = height()-2;
		}
	}
	if (m_x>=width()) m_x=0;
	if (m_y>height()) m_y=height()-2;
#endif
}

void Video::print(const char *cp)
{
	const char *str = cp;
	const char *ch;

	for (ch = str; *ch; ch++) putCh(*ch);
}

void Video::print_prenull(int cnt, int fmtcnt)
{
	if (cnt<fmtcnt) {
        	char s2[256];
		int cc=fmtcnt-cnt;
		if (cc>255) cc=255;
		for (int i=0; i<cc; i++) {
			s2[i] = '0';
		}
		s2[cc] = 0;
        	print(s2);
	}
}

void Video::print_l(long val, int radix, int fmtcnt)
{
        if (val==0) {
		print_prenull(1, fmtcnt);
                putCh('0');
                return;
        }
        bool n = false;
        if (val<0) {
                val *= -1;
                n = true;
        }
        int l = 0;
        int tmp = val;
        while (tmp>0) { tmp/=radix; l++; }
        if (n) l++;
        if (l==0) l=1;

	int cnt = 0;
        char s[256];
        if (n) s[0]='-';
        s[l]=0;
        l--;
        while (l>=0) {
                char t = val%radix;
                if (t<10) s[l]='0'+t;
                else s[l]='A'+t-10;
                val/=radix;
                l--;
		cnt++;
        } 
	print_prenull(cnt, fmtcnt);
        print(s);
}

void Video::print_ul(unsigned long val, int radix, int fmtcnt)
{
        if (val==0) {
		print_prenull(1, fmtcnt);
                putCh('0');
                return;
        }
        int l = 0;
        unsigned int long tmp = val;
        while (tmp>0) { tmp/=radix; l++; }
        if (l==0) l=1;

	int cnt = 0;
        char s[256];
        s[l]=0;
        s[l+1]=0;
        l--;
        while (l>=0) {
                char t = val%radix;
                if (t<10) s[l]='0'+t;
                else s[l]='A'+(t-10);
                val/=radix;
                l--;
		cnt++;
        } 
	print_prenull(cnt, fmtcnt);
        print(s);
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
	int fmtcnt = 0;

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
                                print_ul(va_arg(al, unsigned int), 16, fmtcnt);
                                f = false;
                        }
                        else if (*ch=='c') {
				putCh(va_arg(al, unsigned int));
                                f = false;
			}
                        else if (*ch=='d') {
                                if (!s_signed) {
                                        print_ul(va_arg(al, unsigned int), 10, fmtcnt);
                                } else {
                                        print_l(va_arg(al, int), 10, fmtcnt);
                                }
                                f = false;
                        } else if (*ch=='u') {
                                if (l_cnt==0) print_ul(va_arg(al, unsigned int), 10, fmtcnt);
                                else if (l_cnt==1) print_ul(va_arg(al, unsigned long), 10, fmtcnt);
                                else if (l_cnt==2) print_ul(va_arg(al, unsigned long long), 10, fmtcnt);
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
			else if (*ch>='0' && *ch<='9') {
				fmtcnt *= 10;
				fmtcnt += (*ch-'0');
			}
                } 
                else if (*ch=='%') {
			fmtcnt = 0;
                        f = true;
                        s_signed = true;
                }
		else if (*ch=='\r') {
			m_x = 0;
		}
		else if (*ch=='\n') {
			m_x = 0;
			m_y++;
			putCh(*ch);
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
	if (c=='\n') return;
	if (m_x>=width()) {
		m_x -= width();
		m_y++;
	}

	scroll();

	if (Platform::fb()!=NULL && Platform::fb()->isConfigured()) {
#if 1
		if (m_font==NULL) {
			m_font = new KernelFont();
			//clear();
			m_x = 0;
			m_y = 0;
		}
		m_font->drawFont(Platform::fb(), m_x*m_font->width(), m_y*(m_font->height()+0), c);
		m_x++;
#endif
	} else {
#if 1
		if (m_videomem==NULL) return;
		unsigned int offset = m_y*width() + m_x; 

		m_videomem[offset] = c | VIDEO_COLOR_MASK;

		m_x++;
		setCursor();
#endif
	}

#if 0
	//volatile unsigned int a;
	unsigned int a;
	for (a=0; a<0x1ffffff; a++) { }
#endif
}
