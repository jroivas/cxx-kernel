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

double exp(double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fldl2e ; fmulp ; f2xm1" : "+t"(x));
    return x + 1;
#else
#warning exp function not available for this platform
#endif
}

float expf(float x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fldl2e ; fmulp ; f2xm1" : "+t"(x));
    return x + 1;
#else
#warning expf function not available for this platform
#endif
}

long double expl(long double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fldl2e ; fmulp ; f2xm1" : "+t"(x));
    return x + 1;
#else
#warning expl function not available for this platform
#endif
}

double exp2(double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("f2xm1" : "+t"(x));
    return x + 1;
#else
#warning exp2 function not available for this platform
#endif
}

float exp2f(float x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("f2xm1" : "+t"(x));
    return x + 1;
#else
#warning exp2f function not available for this platform
#endif
}

long double exp2l(long double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("f2xm1" : "+t"(x));
    return x + 1;
#else
#warning exp2l function not available for this platform
#endif
}

double expm1(double x) {
    return exp(x) - 1;
}

float expm1f(float x) {
    return expf(x) - 1;
}

long double expm1l(long double x) {
    return expl(x) - 1;
}

double frexp(double x, int* exp);
float frexpf(float x, int* exp);
long double frexpl(long double x, int* exp);
int ilogb(double x);
int ilogbf(float x);
int ilogbl(long double x);

double ldexp(double x, int exp) {
    return x * (1 << exp);
}

float ldexpf(float x, int exp) {
    return x * (1 << exp);
}

long double ldexpl(long double x, int exp) {
    return x * (1 << exp);
}

double log2(double x);
float log2f(float x);
long double log2l(long double x);
double log1p(double x);
float log1pf(float x);
long double log1pl(long double x);

double log(double x) {
    double ln2;
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fldln2" : "=t"(ln2));
#else
#warning log function not available for this platform
#endif
    return ln2 * log2(x);
}

float logf(float x) {
    float ln2;
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fldln2" : "=t"(ln2));
#else
#warning logf function not available for this platform
#endif
    return ln2 * log2f(x);
}

long double logl(long double x) {
    long double ln2;
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fldln2" : "=t"(ln2));
#else
#warning log function not available for this platform
#endif
    return ln2 * log2l(x);
}

double log10(double x) {
    double log10_2;
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fldlg2" : "=t"(log10_2));
#else
#warning log10 function not available for this platform
#endif
    return log10_2 * log2(x);
}

float log10f(float x) {
    double log10_2;
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fldlg2" : "=t"(log10_2));
#else
#warning log10f function not available for this platform
#endif
    return log10_2 * log2f(x);
}

long double log10l(long double x) {
    double log10_2;
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fldlg2" : "=t"(log10_2));
#else
#warning log10l function not available for this platform
#endif
    return log10_2 * log2l(x);
}

double log1p(double x) {
    return log(1 + x);
}

float log1pf(float x) {
    return logf(1 + x);
}

long double log1pl(long double x) {
    return logl(1 + x);
}

double log2(double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fld1 ; fxch ; fyl2x" : "+t"(x));
    return x;
#else
#warning log2 function not available for this platform
#endif
}

float log2f(float x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fld1 ; fxch ; fyl2x" : "+t"(x));
    return x;
#else
#warning log2f function not available for this platform
#endif
}

long double log2l(long double x) {
#if defined(__GNUC__) && defined(_TARGET_X86_)
    asm("fld1 ; fxch ; fyl2x" : "+t"(x));
    return x;
#else
#warning log2l function not available for this platform
#endif
}

double logb(double x);
float logbf(float x);
long double logbl(long double x);
double modf(double value, double* iptr);
float modff(float value, float* iptr);
long double modfl(long double value, long double* iptr);
double scalbn(double x, int n);
float scalbnf(float x, int n);
long double scalbnl(long double x, int n);
double scalbln(double x, long int n);
float scalblnf(float x, long int n);
long double scalblnl(long double x, long int n);
