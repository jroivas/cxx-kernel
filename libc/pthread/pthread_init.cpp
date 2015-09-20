#include <pthread_impl.h>
#include <mm.h>

#if 0
__thread struct pthread *__my_pthread = 0;
extern "C" struct pthread *__pthread_self()
{
    printf("__pthread_self1\n");
    if (__my_pthread == 0) {
        printf("__pthread_self2\n");
        __my_pthread = (__pthread*)calloc(sizeof(struct pthread), 1);
    }
    printf("__pthread_self3\n");
    return __my_pthread;
}

#endif
