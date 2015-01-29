#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int app_main()
{
    printf("Hello, world!\n");

    char buf[10];
    int fd = open("/dev/random", O_RDONLY);

    printf("fd: %d\n", fd);
    if (fd >= 0) {
        int res = read(fd, buf, 3);
        printf("res: %d\n", res);

        printf("Data: %d %d %d\n", buf[0], buf[1], buf[2]);
    }

    close(fd);

    return 0;
}
