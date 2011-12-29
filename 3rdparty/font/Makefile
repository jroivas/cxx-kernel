include ../../config.mk

all: boot_font.o

boot_font.o: boot_font.c raw_font.h

.c.o:
	$(CC) -c $(CFLAGS) -I../.. -o $@ $< 


clean:
	rm -rf *.o
