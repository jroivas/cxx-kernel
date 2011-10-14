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
	virtual ModeConfig *query(ModeConfig prefer) = 0;
	virtual bool configure(ModeConfig *mode) = 0;
	virtual void blit() = 0;
	void swap();
	unsigned char *data();

protected:
	unsigned char *buffer;
	unsigned char *backbuffer;
	ModeConfig *current;
};
#endif