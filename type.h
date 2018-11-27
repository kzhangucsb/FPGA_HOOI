#include "ap_fixed.h"

#ifdef __SYNTHESIS__
typedef ap_fixed<16, 2> _float;
typedef ap_fixed<24, 10> _float_bigacc;
typedef ap_fixed<16, 10> _float_smlacc;
typedef ap_fixed<16, 3> _angle;
typedef ap_fixed<15, 2> _sincos;
#else
typedef float _float;
typedef float _float_bigacc;
typedef float _float_smlacc;
typedef float _angle;
typedef float _sincos;
//#define _float float
#endif

//#define numel_inner// 256
//#define numel_outer //400
//#define dim //30
//#define dim2 //5
//#define dim2 8

#define rank 8
#define ttmbatsize 32
#define tensize 128

