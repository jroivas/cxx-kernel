#ifndef FB_H
#define FB_H

class FB
{
public:
	struct ModeConfig {
		int width:32;
		int height:32;
		unsigned short bytes_per_line:16;
		unsigned char depth:8;
		unsigned char unused:8;
		unsigned char rsize;
		unsigned char gsize;
		unsigned char bsize;
		unsigned char asize;
	};

	FB();
	~FB();
	virtual ModeConfig *query(ModeConfig *prefer) = 0;
	virtual bool configure(ModeConfig *mode);
	virtual void blit() = 0;
	void swap();
	void clear();
	unsigned char *data();
	void setSingleBuffer();
	void putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
	void putPixel(int x, int y, unsigned int color);

protected:
	void allocBuffers();
	void freeBuffers();
	unsigned char *buffer;
	unsigned char *backbuffer;
	bool double_buffer;
	ModeConfig *current;
	int tmp_w;
	int tmp_h;
};
#endif
