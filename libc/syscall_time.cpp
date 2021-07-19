#include "syscall_time.h"
#include <platform.h>
#include <errno.h>
#include <time.h>

extern "C" long long __tm_to_secs(const struct tm *tm);

int syscall_clock_gettime(clockid_t clockid, struct timespec *tp)
{
    Realtime::TimeInfo time;
    //Platform::video()->printf(">> gettime %d\n", clockid);

    Mem::set(tp, 0, sizeof(struct timespec));

    switch (clockid) {
    case CLOCK_REALTIME:
        {
            struct tm t = {};
            time = Platform::realtime()->getTime();

            /*
             * Hack to convert time to unix timestamp. Utilize
             * __tm_to_secs() from musl to do the heavy lifting.
             * Just make sure the date/time is in right format.
             */
            t.tm_sec = time.seconds;
            t.tm_min = time.minutes;
            t.tm_hour = time.hours;
            t.tm_mday = time.day;
            t.tm_mon = time.month;
            t.tm_year = time.year - 1900;

            long long tmp = __tm_to_secs(&t);
            tp->tv_sec = tmp;
            tp->tv_nsec = tmp * 1000 * 1000;
        }
#if 0
        Platform::video()->printf("Now is %u-%u-%u %.2u:%.2u:%.2u\n",
            time.year, time.month, time.day,
            time.hours, time.minutes, time.seconds);
#endif
        return 0;
    default:
        break;
    };

    return -EINVAL;
}
