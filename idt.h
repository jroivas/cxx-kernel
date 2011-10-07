#ifndef IDT_H
#define IDT_H

class IDT
{
public:
	IDT();
	void initISR();
	void initIRQ();

	struct Ptr {
		unsigned short limit;
		unsigned int base;
	};

private:
	struct Entry {
		unsigned short base_low;
		unsigned short sel;
		unsigned char always0;
		unsigned char flags;
		unsigned short base_high;
	};

	Entry idt[256];
	Ptr idtp;

	void load();
};

#endif
