#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int app_main()
{
    printf("Randomizing\n");

    char buf[20];
    int fd = open("/dev/random", O_RDONLY);

    printf("fd: %d\n", fd);
    if (fd >= 0) {
        int res = read(fd, buf, 10);
        printf("res: %d\n", res);
        printf("Data: ");

        for (int i = 0; i < res; ++i) {
            printf("%u ", (unsigned char)buf[i]);
        }
        printf("\n");
    }

    close(fd);

    return 0;
}
