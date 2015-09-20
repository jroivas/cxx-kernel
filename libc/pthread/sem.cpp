#include <semaphore.h>

int sem_timedwait(sem_t* s, const struct timespec *abs_timeout)
{
    (void)s;
    (void)abs_timeout;
    return 0;
}
