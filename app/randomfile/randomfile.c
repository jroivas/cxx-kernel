#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Randomizing..\n");

    char buf[20];
    int fd = open("/dev/urandom", O_RDONLY);
    int fd2 = open("/dev/urandom", O_RDONLY);

    FILE *fd3 = fopen("/dev/urandom", "r");

    printf("fd : %d\n", fd);
    printf("fd2: %d\n", fd2);
    printf("fd3: %d\n", fd3);
    if (fd >= 0) {
        int res = read(fd, buf, 10);
        printf("res: %d\n", res);

        printf("Data: ");
        for (int i = 0; i < res; ++i) {
            printf("%u ", (unsigned char)buf[i]);
        }
        printf("\n");
    }
    if (fd3 != NULL) {
        int a = 1;
        int b = 10;
        int res = fread(buf, a, b, fd3);
        printf("res: %d\n", res);

        printf("Data: ");
        for (int i = 0; i < a * b; ++i) {
            printf("%u ", (unsigned char)buf[i]);
        }
        printf("\n");
        fclose(fd3);
    }

    close(fd);
    close(fd2);

    return 0;
}
