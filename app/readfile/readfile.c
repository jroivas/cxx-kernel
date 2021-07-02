#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    //FILE *fd = fopen("/cfs/test", "r");
    //FILE *fd = fopen("/ustar/test", "r");
    FILE *fd = fopen("/ustar/lipsum.txt", "r");

    printf("fopen fd: %d\n", fd);
    if (fd != NULL) {
        char buf[256];
        const int cnt = 256;
        int res = 0;
        printf("Data: \n");
        while (1) {
            res = fread(buf, 1, cnt, fd);
            //res = fread(buf, cnt, 1, fd);
            if (feof(fd) || res <= 0)
                break;

            for (int i = 0; i < cnt; ++i) {
                //printf("%x %c\n", (unsigned char)buf[i], (unsigned char)buf[i]);
                printf("%c", (unsigned char)buf[i]);
            }
        }
        printf("\n");

        fclose(fd);
    }

    return 0;
}
