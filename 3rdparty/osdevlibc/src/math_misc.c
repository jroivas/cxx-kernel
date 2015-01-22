/*

   Copyright 2009 Pierre KRIEGER

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#include <math.h>
#include <stdint.h>

double erf(double x);
float erff(float x);
long double erfl(long double x);
double erfc(double x);
float erfcf(float x);
long double erfcl(long double x);
double lgamma(double x);
float lgammaf(float x);
long double lgammal(long double x);
double tgamma(double x);
float tgammaf(float x);
long double tgammal(long double x);
double nearbyint(double x);
long double nearbyintl(long double x);
float nearbyintf(float x);

#ifdef __i386__
    #define _TARGET_X86_
#endif

double ceil(double x) {
#ifdef _TARGET_X86_
    // first we have to modify the CR field in the x87 control register
    uint16_t controlWord;
    asm volatile("fstcw %0" : : "m"(controlWord) : "memory");
    controlWord |= 0x400; controlWord &= ~0x800;
    asm volatile("fldcw %0" : : "m"(controlWord));
    return nearbyintl(x);
#else
#warning ceil function not supported for this platform
#endif
}

float ceilf(float x) {
#ifdef _TARGET_X86_
    // first we have to modify the CR field in the x87 control register
    uint16_t controlWord;
    asm volatile("fstcw %0" : : "m"(controlWord) : "memory");
    controlWord |= 0x400; controlWord &= ~0x800;
    asm volatile("fldcw %0" : : "m"(controlWord));
    return nearbyintl(x);
#else
#warning ceilf function not supported for this platform
#endif
}

long double ceill(long double x) {
#ifdef _TARGET_X86_
    // first we have to modify the CR field in the x87 control register
    uint16_t controlWord;
    asm volatile("fstcw %0" : : "m"(controlWord) : "memory");
    controlWord |= 0x400; controlWord &= ~0x800;
    asm volatile("fldcw %0" : : "m"(controlWord));
    return nearbyintl(x);
#else
#warning ceill function not supported for this platform
#endif
}

double floor(double x) {
#ifdef _TARGET_X86_
    // first we have to modify the CR field in the x87 control register
    uint16_t controlWord;
    asm volatile("fstcw %0" : : "m"(controlWord) : "memory");
    controlWord |= 0x800; controlWord &= ~0x400;
    asm volatile("fldcw %0" : : "m"(controlWord));
    return nearbyint(x);
#else
#warning floor function not supported for this platform
#endif
}

float floorf(float x) {
#ifdef _TARGET_X86_
    // first we have to modify the CR field in the x87 control register
    uint16_t controlWord;
    asm volatile("fstcw %0" : : "m"(controlWord) : "memory");
    controlWord |= 0x800; controlWord &= ~0x400;
    asm volatile("fldcw %0" : : "m"(controlWord));
    return nearbyintf(x);
#else
#warning floorf function not supported for this platform
#endif
}

long double floorl(long double x) {
#ifdef _TARGET_X86_
    // first we have to modify the CR field in the x87 control register
    uint16_t controlWord;
    asm volatile("fstcw %0" : : "m"(controlWord) : "memory");
    controlWord |= 0x800; controlWord &= ~0x400;
    asm volatile("fldcw %0" : : "m"(controlWord));
    return nearbyintl(x);
#else
#warning floorl function not supported for this platform
#endif
}

double nearbyint(double x) {
#ifdef _TARGET_X86_
    asm("frndint" : "+t"(x));
    return x;
#else
#warning nearbyint function not supported for this platform
#endif
}

float nearbyintf(float x) {
#ifdef _TARGET_X86_
    asm("frndint" : "+t"(x));
    return x;
#else
#warning nearbyintf function not supported for this platform
#endif
}

long double nearbyintl(long double x) {
#ifdef _TARGET_X86_
    asm("frndint" : "+t"(x));
    return x;
#else
#warning nearbyintl function not supported for this platform
#endif
}

double rint(double x);
float rintf(float x);
long double rintl(long double x);
long int lrint(double x);
long int lrintf(float x);
long int lrintl(long double x);
long long int llrint(double x);
long long int llrintf(float x);
long long int llrintl(long double x);
double round(double x);
float roundf(float x);
long double roundl(long double x);
long int lround(double x);
long int lroundf(float x);
long int lroundl(long double x);
long long int llround(double x);
long long int llroundf(float x);
long long int llroundl(long double x);
double trunc(double x);
float truncf(float x);
long double truncl(long double x);
double fmod(double x, double y);
float fmodf(float x, float y);
long double fmodl(long double x, long double y);

double remainer(double x, double y) {
    double result;
#ifdef _TARGET_X86_
    asm("fld %2 ; fld %1 ; fprem ; fxch ; fincstp" : "=t"(result) : "m"(x), "m"(y));
#else
#warning remainer function not supported for this platform
#endif
    return result;
}

float remainerf(float x, float y) {
    float result;
#ifdef _TARGET_X86_
    asm("fld %2 ; fld %1 ; fprem ; fxch ; fincstp" : "=t"(result) : "m"(x), "m"(y));
#else
#warning remainerf function not supported for this platform
#endif
    return result;
}

long double remainerl(long double x, long double y) {
    long double result;
#ifdef _TARGET_X86_
    asm("fld %2 ; fld %1 ; fprem ; fxch ; fincstp" : "=t"(result) : "m"(x), "m"(y));
#else
#warning remainerl function not supported for this platform
#endif
    return result;
}

double remquo(double x, double y, int* quo) {
    *quo = (int)x / (int)y;
    return remainer(x, y);
}

float remquof(float x, float y, int* quo) {
    *quo = (int)x / (int)y;
    return remainerf(x, y);
}

long double remquol(long double x, long double y, int* quo) {
    *quo = (int)x / (int)y;
    return remainerl(x, y);
}

double copysign(double x, double y) {
    double magnitude = fabs(x);
    return signbit(y) ? -magnitude : magnitude;
}

float copysignf(float x, float y) {
    double magnitude = fabsf(x);
    return signbit(y) ? -magnitude : magnitude;
}

long double copysignl(long double x, long double y) {
    double magnitude = fabsl(x);
    return signbit(y) ? -magnitude : magnitude;
}

double nan(const char* tagp);
float nanf(const char* tagp);
long double nanl(const char* tagp);
double nextafter(double x, double y);
float nextafterf(float x, float y);
long double nextafterl(long double x, long double y);
double nexttoward(double x, long double y);
float nexttowardf(float x, long double y);
long double nexttowardl(long double x, long double y);

double fdim(double x, double y) {
    return (x > y) ? (x - y) : 0;
}

float fdimf(float x, float y) {
    return (x > y) ? (x - y) : 0;
}

long double fdiml(long double x, long double y) {
    return (x > y) ? (x - y) : 0;
}

double fmax(double x, double y) {
    return (x > y) ? x : y;
}

float fmaxf(float x, float y) {
    return (x > y) ? x : y;
}

long double fmaxl(long double x, long double y) {
    return (x > y) ? x : y;
}

double fmin(double x, double y) {
    return (x < y) ? x : y;
}

float fminf(float x, float y) {
    return (x < y) ? x : y;
}

long double fminl(long double x, long double y) {
    return (x < y) ? x : y;
}

double fma(double x, double y, double z) {
    return x * y + z;
}

float fmaf(float x, float y, float z) {
    return x * y + z;
}

long double fmal(long double x, long double y, long double z) {
    return x * y + z;
}

