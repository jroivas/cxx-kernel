#ifndef VIDEO_H
#define VIDEO_H

class Video
{
public:
	Video();
	~Video();

	void clear();
	void print(const char *cp);
	void putCh(char c);

	void resize(int width, int height);

	inline unsigned int size();
	inline unsigned int width();
	inline unsigned int height();

private:
	unsigned short *m_videomem;
	unsigned int m_x;
	unsigned int m_y;

	unsigned int m_width;
	unsigned int m_height;
};

#endif
