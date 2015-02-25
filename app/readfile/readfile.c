#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    char buf[50];
    FILE *fd = fopen("/cfs/test", "r");

    printf("fopen fd: %d\n", fd);
    if (fd != NULL) {
        int cnt = 36;
        int res = fread(buf, 1, cnt, fd);
        printf("res: %d\n", res);

        printf("Data: ");
        for (int i = 0; i < res; ++i) {
            //printf("%x %c\n", (unsigned char)buf[i], (unsigned char)buf[i]);
            printf("%c", (unsigned char)buf[i]);
        }
        printf("\n");
    }

    fclose(fd);

    return 0;
}
