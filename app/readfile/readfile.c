#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    //FILE *fd = fopen("/cfs/test", "r");
    //FILE *fd = fopen("/ustar/test", "r");
    //FILE *fd = fopen("/ustar/lipsum.txt", "r");
    FILE *fd = fopen("/ustar/hello.py", "r");

    printf("fopen fd: %d\n", fd);
    if (fd != NULL) {
        char buf[256];
        const int cnt = 20;
        int res = 0;
        struct stat sb = {0};
        int fno;

        (void)sb;
        (void)fno;
        fno = fileno(fd);
        printf("fno    : %u\n", fno);
        fstat(fno, &sb);

        printf("Size   : %u\n", sb.st_size);
        printf("Blksize: %u\n", sb.st_blksize);
        printf("Blocks : %u\n", sb.st_blocks);

        printf("Data: \n");
        while (1) {
            res = fread(buf, 1, cnt, fd);
            //res = fread(buf, cnt, 1, fd);
            if (res <= 0)
                break;

            //printf("-- %d\n", res);
            for (int i = 0; i < res; ++i) {
                //printf("%x %c\n", (unsigned char)buf[i], (unsigned char)buf[i]);
                //printf("%c", (unsigned char)buf[i]);
                printf("%u. %x %c\n", i, (unsigned char)buf[i], buf[i]);
            }
        }
        printf("\n");

        fclose(fd);
    }

    return 0;
}
