#ifndef MATH_H
#define MATH_H

#define ABS_STUB(j) if (j<0) j*=-1; return j;

#ifdef __cplusplus
extern "C" {
#endif
int abs(int j);
long int labs(long int j);
long long int llabs(long long int j);

#ifdef __cplusplus
}
#endif

#endif
