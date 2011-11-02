#include "types.h"

#define ABS_STUB(j) if (j<0) j*=-1; return j;

int abs(int j) { ABS_STUB(j); }
long int labs(long int j) { ABS_STUB(j); }
long long int llabs(long long int j) { ABS_STUB(j); }

uint64_t __udivmoddi4 (uint64_t num, uint64_t den, uint64_t *rem_p)		
{
  uint64_t quot = 0, qbit = 1;
 
   if ( den == 0 ) {
     return 1/((unsigned)den); /* Intentional divide by zero, without
                                  triggering a compiler warning which
                                  would abort the build */
   }
 
   /* Left-justify denominator and count shift */
   while ( (int64_t)den >= 0 ) {
     den <<= 1;
     qbit <<= 1;
   }
 
   while ( qbit ) {
     if ( den <= num ) {
       num -= den;
       quot += qbit;
     }
     den >>= 1;
     qbit >>= 1;
   }
 
   if ( rem_p )
     *rem_p = num;
 
   return quot;
}

extern "C" uint64_t my__udivdi3(uint64_t num, uint64_t den)
{
   return __udivmoddi4(num, den, NULL);
}

extern "C" uint64_t my__umoddi3(uint64_t num, uint64_t den)
{
  uint64_t v;

  (void) __udivmoddi4(num, den, &v);
  return v;
}

extern "C" int64_t my__divdi3(int64_t num, int64_t den)
{
  int minus = 0;
  int64_t v;

  if ( num < 0 ) {
    num = -num;
    minus = 1;
  }
  if ( den < 0 ) {
    den = -den;
    minus ^= 1;
  }

  v = __udivmoddi4(num, den, NULL);
  if ( minus )
    v = -v;

  return v;
}

extern "C" int64_t my__moddi3(int64_t num, int64_t den)
{
  int minus = 0;
  int64_t v;

  if ( num < 0 ) {
    num = -num;
    minus = 1;
  }
  if ( den < 0 ) {
    den = -den;
    minus ^= 1;
  }

  (void) __udivmoddi4(num, den, (uint64_t *)&v);
  if ( minus )
    v = -v;

  return v;
}

#if 0
#define do_div(n, base)                                         \
({                                                              \
        unsigned long __upper, __low, __high, __mod, __base;    \
        __base = (base);                                        \
        asm("":"=a" (__low), "=d" (__high) : "A" (n));          \
        __upper = __high;                                       \
        if (__high) {                                           \
                __upper = __high % (__base);                    \
                __high = __high / (__base);                     \
        }                                                       \
        asm("divl %2":"=a" (__low), "=d" (__mod)                \
            : "rm" (__base), "" (__low), "1" (__upper));        \
        asm("":"=A" (n) : "a" (__low), "d" (__high));           \
        __mod;                                                  \
})
#endif

#if 0
static inline int fls(int x)
{
        int r;
#if 1
        asm("bsrl %1,%0\n\t"
            "cmovzl %2,%0"
            : "=&r" (r) : "rm" (x), "rm" (-1));
#else
        asm("bsrl %1,%0\n\t"
            "jnz 1f\n\t"
            "movl $-1,%0\n"
            "1:" : "=r" (r) : "rm" (x));
#endif
        return r + 1;
}

# define do_div(n,base) ({                              \
        uint32_t __base = (base);                       \
        uint32_t __rem;                                 \
        (void)(((typeof((n)) *)0) == ((uint64_t *)0));  \
        if (((n) >> 32) == 0) {                         \
                __rem = (uint32_t)(n) % __base;         \
                (n) = (uint32_t)(n) / __base;           \
        } else                                          \
                __rem = __div64_32(&(n), __base);       \
        __rem;                                          \
 })

uint32_t __div64_32(uint64_t *n, uint32_t base)
{
        uint64_t rem = *n;
        uint64_t b = base;
        uint64_t res, d = 1;
        uint32_t high = rem >> 32;

        /* Reduce the thing a bit first */
        res = 0;
        if (high >= base) {
                high /= base;
                res = (uint64_t) high << 32;
                rem -= (uint64_t) (high*base) << 32;
        }

        while ((int64_t)b > 0 && b < rem) {
                b = b+b;
                d = d+d;
        }

        do {
                if (rem >= b) {
                        rem -= b;
                        res += d;
                }
                b >>= 1;
                d >>= 1;
        } while (d);

        *n = res;
        return rem;
}

/* 64bit divisor, dividend and result. dynamic precision */
//uint64_t div64_64(uint64_t dividend, uint64_t divisor)
extern "C" uint64_t __udivdi3(uint64_t dividend, uint64_t divisor)
{
        uint32_t high, d;

        high = divisor >> 32;
        if (high) {
                unsigned int shift = fls(high);

                d = divisor >> shift;
                dividend >>= shift;
        } else
                d = divisor;

        uint64_t mod = do_div(dividend, d);
	(void)mod;

        return dividend;
}

extern "C" uint64_t __umoddi3(uint64_t dividend, uint64_t divisor)
{
        uint32_t high, d;

        high = divisor >> 32;
        if (high) {
                unsigned int shift = fls(high);

                d = divisor >> shift;
                dividend >>= shift;
        } else
                d = divisor;

        uint64_t mod = do_div(dividend, d);

        return mod;
}

extern "C" uint64_t __divdi3(int64_t dividend, int64_t divisor)
{
	return __udivdi3(dividend, divisor);
}

extern "C" uint64_t __moddi3(int64_t dividend, int64_t divisor)
{
	return __umoddi3(dividend, divisor);
}
#endif

#if 0
int nlz64(unsigned long long x) {
   int n;

   if (x == 0) return(64);
   n = 0;
   if (x <= 0x00000000FFFFFFFF) {n = n + 32; x = x << 32;}
   if (x <= 0x0000FFFFFFFFFFFF) {n = n + 16; x = x << 16;}
   if (x <= 0x00FFFFFFFFFFFFFF) {n = n +  8; x = x <<  8;}
   if (x <= 0x0FFFFFFFFFFFFFFF) {n = n +  4; x = x <<  4;}
   if (x <= 0x3FFFFFFFFFFFFFFF) {n = n +  2; x = x <<  2;}
   if (x <= 0x7FFFFFFFFFFFFFFF) {n = n +  1;}
   return n;
}

#define DIVU(u, v) ({/*unsigned long long __v = (v) & 0xFFFFFFFF;*/ \
   long long __q = (u)/(v); \
   __q > 0xFFFFFFFF ? 0xdeaddeadbeefbeef : \
   __q | 0xdeadbeef00000000;})
#define DIVS(u, v) ({long long __v = (v) << 32 >> 32; \
   long long __q = (u)/(__v); \
   __q < (long long)0xFFFFFFFF80000000ll || \
   __q > (long long)0x000000007FFFFFFFll ? \
   0xfadefadedeafdeaf : __q | 0xfadedeaf00000000;})

extern "C" uint64_t __udivdi3(uint64_t u, uint64_t v)
{
   unsigned long long u0, u1, v1, q0, q1, k, n;

   if (v >> 32 == 0) {          // If v < 2**32:
      if (u >> 32 < v)          // If u/v cannot overflow,
         return DIVU(u, v)      // just do one division.
            & 0xFFFFFFFF;
      else {                    // If u/v would overflow:
         u1 = u >> 32;          // Break u up into two
         u0 = u & 0xFFFFFFFF;   // halves.
         q1 = DIVU(u1, v)       // First quotient digit.
            & 0xFFFFFFFF;
         k = u1 - q1*v;         // First remainder, < v.
         q0 = DIVU((k << 32) + u0, v) // 2nd quot. digit.
            & 0xFFFFFFFF;
         return (q1 << 32) + q0;
      }
   }
                                // Here v >= 2**32.
   n = nlz64(v);                // 0 <= n <= 31.
   v1 = (v << n) >> 32;         // Normalize the divisor
                                // so its MSB is 1.
   u1 = u >> 1;                 // To ensure no overflow.
   q1 = DIVU(u1, v1)            // Get quotient from
       & 0xFFFFFFFF;            // divide unsigned insn.
   q0 = (q1 << n) >> 31;        // Undo normalization and
                                // division of u by 2.
   if (q0 != 0)                 // Make q0 correct or
      q0 = q0 - 1;              // too small by 1.
   if ((u - q0*v) >= v)
      q0 = q0 + 1;              // Now q0 is correct.
   return q0;
}

extern "C" long long __divdi3(long long u, long long v) {

   unsigned long long au, av;
   long long q, t;

   au = llabs(u);
   av = llabs(v);
   if (av >> 31 == 0) {         // If |v| < 2**31 and
// if (v << 32 >> 32 == v) {    // If v is in range and
      if (au < av << 31) {      // |u|/|v| cannot
         q = DIVS(u, v);        // overflow, use DIVS.
         return (q << 32) >> 32;
      }
   }
   q = au/av;                   // Invoke udivdi3.
   t = (u ^ v) >> 63;           // If u, v have different
   return (q ^ t) - t;          // signs, negate q.
}

#endif
