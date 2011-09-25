#ifndef MUTEX_H
#define MUTEX_H

class Mutex
{
public:
	Mutex();
	Mutex(void *ptr);
	~Mutex();

	void lock();
	bool isLocked();
	void unlock();
	bool wait();
	void abort();

private:
	unsigned char *m_ptr;
};

#endif
