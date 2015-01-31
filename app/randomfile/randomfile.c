#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Randomizing\n");

    char buf[20];
    int fd = open("/dev/urandom", O_RDONLY);
    int fd2 = open("/dev/urandom", O_RDONLY);

    printf("fd : %d\n", fd);
    printf("fd2: %d\n", fd2);
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
    close(fd2);

    return 0;
}
