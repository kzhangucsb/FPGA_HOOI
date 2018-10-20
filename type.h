

#ifdef __SYNTHESIS__
#include "ap_fixed.h"
#define _float ap_fixed<16, 1>
#else
#define _float float
#endif

#define numel_inner 256
#define numel_outer 400
#define dim 30
#define dim2 5
