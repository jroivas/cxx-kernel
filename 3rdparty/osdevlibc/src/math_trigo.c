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

double acos(double x) {
    double pi;
#ifdef _TARGET_X86_
    asm("fldpi" : "=t"(pi));
#else
    pi = 3.14159265358979;
#endif
    return (pi / 2) - asin(x);
}

float acosf(float x) {
    float pi;
#ifdef _TARGET_X86_
    asm("fldpi" : "=t"(pi));
#else
    pi = 3.14159265358979;
#endif
    return (pi / 2) - asinf(x);
}

long double acosl(long double x) {
    long double pi;
#ifdef _TARGET_X86_
    asm("fldpi" : "=t"(pi));
#else
    pi = 3.14159265358979;
#endif
    return (pi / 2) - asinl(x);
}

double asin(double x) {
    return 2. * atan(x / (1. + sqrt(1. - x * x)));
}

float asinf(float x) {
    return 2. * atanf(x / (1. + sqrtf(1. - x * x)));
}

long double asinl(long double x) {
    return 2. * atanl(x / (1. + sqrtl(1. - x * x)));
}

double atan(double x) {
    return atan2(x, 1.);
}

float atanf(float x) {
    return atan2(x, 1.);
}

long double atanl(long double x) {
    return atan2(x, 1.);
}

double atan2(double y, double x) {
#if defined(_TARGET_X86_)
    double result;
    asm("fld %1 ; fld %2 ; fpatan" : "=t"(result) : "m"(y), "m"(x));
    return result;
#else
#warning atan2 function not supported for this platform
#endif
}

float atan2f(float y, float x) {
#if defined(_TARGET_X86_)
    float result;
    asm("fld %1 ; fld %2 ; fpatan" : "=t"(result) : "m"(y), "m"(x));
    return result;
#else
#warning atan2f function not supported for this platform
#endif
}

long double atan2l(long double y, long double x) {
#if defined(_TARGET_X86_)
    long double result;
    asm("fld %1 ; fld %2 ; fpatan" : "=t"(result) : "m"(y), "m"(x));
    return result;
#else
#warning atan2l function not supported for this platform
#endif
}

double cos(double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fcos" : "+t"(x));
    return x;
#else
    double xsquare = x * x;
    return 1 - xsquare / 2 + xsquare * xsquare / 24 - xsquare * xsquare * xsquare / 720 + xsquare * xsquare * xsquare * xsquare / 40320;
#endif
}

float cosf(float x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fcos" : "+t"(x));
    return x;
#else
    float xsquare = x * x;
    return 1 - xsquare / 2 + xsquare * xsquare / 24 - xsquare * xsquare * xsquare / 720 + xsquare * xsquare * xsquare * xsquare / 40320;
#endif
}

long double cosl(long double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fcos" : "+t"(x));
    return x;
#else
    long double xsquare = x * x;
    return 1 - xsquare / 2 + xsquare * xsquare / 24 - xsquare * xsquare * xsquare / 720 + xsquare * xsquare * xsquare * xsquare / 40320;
#endif
}

double sin(double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fsin" : "+t"(x));
    return x;
#else
#warning sin function not supported for this platform
#endif
}

float sinf(float x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fsin" : "+t"(x));
    return x;
#else
#warning sinf function not supported for this platform
#endif
}

long double sinl(long double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fsin" : "+t"(x));
    return x;
#else
#warning sinl function not supported for this platform
#endif
}

double tan(double x) {
    return sin(x) / cos(x);
}

float tanf(float x) {
    return sinf(x) / cosf(x);
}

long double tanl(long double x) {
    return sinl(x) / cosl(x);
}

double acosh(double x) {
    return log(x + sqrt(x * x - 1));
}

float acoshf(float x) {
    return logf(x + sqrtf(x * x - 1));
}

long double acoshl(long double x) {
    return logl(x + sqrtl(x * x - 1));
}

double asinh(double x) {
    return log(x + sqrt(x * x + 1));
}

float asinhf(float x) {
    return logf(x + sqrtf(x * x + 1));
}

long double asinhl(long double x) {
    return logl(x + sqrtl(x * x + 1));
}

double atanh(double x) {
    return 0.5 * log((1. + x) / (1. - x));
}

float atanhf(float x) {
    return 0.5 * logf((1. + x) / (1. - x));
}

long double atanhl(long double x) {
    return 0.5 * logl((1. + x) / (1. - x));
}

double cosh(double x) {
    return (exp(x) + exp(-x)) * 0.5;
}

float coshf(float x) {
    return (exp(x) + exp(-x)) * 0.5;
}

long double coshl(long double x) {
    return (exp(x) + exp(-x)) * 0.5;
}

double sinh(double x) {
    return (exp(x) - exp(-x)) * 0.5;
}

float sinhf(float x) {
    return (exp(x) - exp(-x)) * 0.5;
}

long double sinhl(long double x) {
    return (exp(x) - exp(-x)) * 0.5;
}

double tanh(double x) {
    return sinh(x) / cosh(x);
}

float tanhf(float x) {
    return sinhf(x) / coshf(x);
}

long double tanhl(long double x) {
    return sinhl(x) / coshl(x);
}
