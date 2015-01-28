#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int app_main()
{
    printf("Hello, world!\n");
    int fd = open("/dev/random", O_RDONLY);
    close(fd);

    return 0;
}
