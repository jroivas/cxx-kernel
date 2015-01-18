#include "time.h"
#include "timer.h"

time_t time(time_t *t)
{
    //FIXME
    time_t res = Timer::get()->getTicks();
    if (t != NULL) {
        *t = res;
    }
    return res;
}
