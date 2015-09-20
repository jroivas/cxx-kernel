#include <stdlib.h>
#include "osmath.h"
#include "types.h"

#if 0
#define ABS_STUB(j)\
    if (j < 0) {\
        j *= -1;\
    }\
    return j;

int abs(int j)
{
    ABS_STUB(j);
}
long int labs(long int j)
{
    ABS_STUB(j);
}
long long int llabs(long long int j)
{
    ABS_STUB(j);
}
#endif

uint64_t my__udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem_p)
{
    uint64_t quot = 0, qbit = 1;

    if (den == 0) {
        return 1 / ((unsigned)den); /* Intentional divide by zero, without
                                       triggering a compiler warning which
                                       would abort the build */
    }

    /* Left-justify denominator and count shift */
    while ( (int64_t)den >= 0 ) {
        den <<= 1;
        qbit <<= 1;
    }

    while (qbit) {
        if (den <= num) {
            num -= den;
            quot += qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }

    if (rem_p) *rem_p = num;

    return quot;
}

extern "C" uint64_t my__udivdi3(uint64_t num, uint64_t den)
{
    return my__udivmoddi4(num, den, NULL);
}

extern "C" uint64_t my__umoddi3(uint64_t num, uint64_t den)
{
    uint64_t v = 0;

    (void) my__udivmoddi4(num, den, &v);
    return v;
}

extern "C" int64_t my__divdi3(int64_t num, int64_t den)
{
    int minus = 0;
    int64_t v;

    if (num < 0) {
        num = -num;
        minus = 1;
    }
    if (den < 0) {
        den = -den;
        minus ^= 1;
    }

    v = my__udivmoddi4(num, den, NULL);

    if (minus) v = -v;

    return v;
}

extern "C" int64_t my__moddi3(int64_t num, int64_t den)
{
    int minus = 0;
    int64_t v = 0;

    if (num < 0) {
        num = -num;
        minus = 1;
    }
    if (den < 0) {
        den = -den;
        minus ^= 1;
    }

    (void) my__udivmoddi4(num, den, (uint64_t *)&v);

    if (minus) v = -v;

    return v;
}

#if 1
extern "C" unsigned int __aeabi_uidiv(unsigned int num, unsigned int den)
{
    unsigned int x = 0;
    unsigned int c = 0;
    for (x = den; x <= num; x += den) c++;
    return c;
}

extern "C" unsigned int __aeabi_uidivmod(unsigned int num, unsigned int den)
{
    unsigned int x = 0;
    unsigned int c = 0;

    for (x = den; x < num; x += den) c++;
    return (num-c*den);
}

extern "C" int __aeabi_idiv(int num, int den)
{
    return __aeabi_uidiv(num, den);
}

extern "C" int __aeabi_idivmod(int num, int den)
{
    return __aeabi_uidivmod(num, den);
}

int64_t __divdi3(int64_t num, int64_t den)
{
    return my__divdi3(num, den);
}

int64_t __moddi3(int64_t num, int64_t den)
{
    return my__moddi3(num, den);
}

uint64_t __umoddi3(uint64_t num, uint64_t den)
{
    return my__umoddi3(num, den);
}

uint64_t __udivdi3(uint64_t num, uint64_t den)
{
    return my__udivdi3(num, den);
}

lldiv_t lldiv(long long int numer, long long int denom)
{
    lldiv_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}
#endif
