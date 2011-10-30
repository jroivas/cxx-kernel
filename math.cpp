#include "math.h"

#define ABS_STUB(j) if (j<0) j*=-1; return j;

int abs(int j) { ABS_STUB(j); }
long int labs(long int j) { ABS_STUB(j); }
long long int llabs(long long int j) { ABS_STUB(j); }
