#ifndef VIDEO_H
#define VIDEO_H

#include "font.h"

class Video
{
public:
	Video();
	virtual ~Video();

	static Video *get();
	void clear();
        void printf(const char *fmt, ...);
	void print(const char *cp);
	virtual void putCh(char c);

	void resize(int width, int height);
	void scroll();

	unsigned int size();
	unsigned int width();
	unsigned int height();

protected:
	void print_ul(unsigned long val, int radix=10, int fmtcnt=0);
        void print_l(long val, int radix=10, int fmtcnt=0);
	void print_prenull(int cnt, int fmtcnt);

	virtual void setCursor() = 0;
	unsigned short *m_videomem;
	unsigned int m_x;
	unsigned int m_y;
	unsigned int m_width;
	unsigned int m_height;
	Font *m_font;
};

#endif
