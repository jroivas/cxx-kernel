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

#ifdef __i386__
    #define _TARGET_X86_
#endif

double cbrt(double x) {
    return pow(x, 1. / 3.);
}

float cbrtf(float x) {
    return powf(x, 1. / 3.);
}

long double cbrtl(long double x) {
    return powl(x, 1. / 3.);
}

double fabs(double x) {
#ifdef _TARGET_X86_
    asm("fabs" : "+t"(x));
    return x;
/*#elif defined(_TARGET_ARM_)
    asm("fmdrr %%d0, %1, %%r1 ; fabsd %%d0, %%d0 ; fmrs %0, %%s0" : "=r"(x) : "r"(x) : "%d0");
    return x;*/
#else
#warning fabs function not available for this platform
#endif
}

float fabsf(float x) {
#ifdef _TARGET_X86_
    asm("fabs" : "+t"(x));
    return x;
/*#elif defined(_TARGET_ARM_)
    asm("fmsr %%s0, %1 ; fabss %%s0, %%s0 ; fmrs %0, %%s0" : "=r"(x) : "r"(x) : "%s0");
    return x;*/
#else
#warning fabsf function not available for this platform
#endif
}

long double fabsl(long double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fabs" : "+t"(x));
    return x;
#else
#warning fabsl function not available for this platform
#endif
}

double hypot(double x, double y) {
    return sqrt(x*x + y*y);
}

float hypotf(float x, float y) {
    return sqrt(x*x + y*y);
}

long double hypotl(long double x, long double y) {
    return sqrt(x*x + y*y);
}

double pow(double x, double y) {
    return exp(y * log(x));
}

float powf(float x, float y) {
    return exp(y * log(x));
}

long double powl(long double x, long double y) {
    return exp(y * log(x));
}

double sqrt(double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fsqrt" : "+t"(x));
    return x;
#else
    return pow(x, 0.5);
#endif
}

float sqrtf(float x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fsqrt" : "+t"(x));
    return x;
#else
    return powf(x, 0.5);
#endif
}

long double sqrtl(long double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fsqrt" : "+t"(x));
    return x;
#else
    return powl(x, 0.5);
#endif
}
