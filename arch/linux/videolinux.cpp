#include "videolinux.h"
#include <stdio.h>

VideoLinux::VideoLinux() : Video()
{
    m_videomem = NULL;
}

void VideoLinux::setCursor()
{
}

void VideoLinux::putCh(char c)
{
    if (c=='\n') {
        ::printf("\n");
    } else {
        ::printf("%c",c);
    }
    Video::putCh(c);
}
