

#ifdef __SYNTHESIS__
#include "ap_fixed.h"
#define _float ap_fixed<16, 2>
#else
#define _float float
#endif

#define numel_inner 128
#define numel_outer 128*128
#define dim 128
#define dim2 8
