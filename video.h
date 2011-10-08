#ifndef VIDEO_H
#define VIDEO_H

class Video
{
public:
	Video();
	~Video();

	void clear();
        void printf(const char *fmt, ...);
	void print(const char *cp);
	void putCh(char c);

	void resize(int width, int height);
	void scroll();

	inline unsigned int size();
	inline unsigned int width();
	inline unsigned int height();

private:
	void print_ul(unsigned long val, int radix=10);
        void print_l(long val, int radix=10);
#if 0
	void print_ld(unsigned int val) { print_lu(val); }
        void print_d(int val) { print_u(val); }
#endif
	void setCursor();
	unsigned short *m_videomem;
	unsigned int m_x;
	unsigned int m_y;

	unsigned int m_width;
	unsigned int m_height;
};

#endif
