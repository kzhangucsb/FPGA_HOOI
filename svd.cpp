#include "type.h"
#include "hls_math.h"

#define maxiter 10

#define dim_n 128
#define dim_m 64

void svd(_float A[dim_n][dim_m], _float U[dim_n][dim_n]) {
#pragma HLS ARRAY_RESHAPE variable=A complete dim=2
#pragma HLS ARRAY_RESHAPE variable=U complete dim=2
//#pragma HLS RESOURCE variable=A core=RAM_1P
//#pragma HLS RESOURCE variable=S core=RAM_1P

	//_bool iter_stop;
	_float a1[dim_m];
	_float a2[dim_m];
	_float b1[dim_m];
	_float b2[dim_m];

	_float s1[dim_n];
	_float s2[dim_n];
	_float t1[dim_n];
	_float t2[dim_n];

	#pragma HLS ARRAY_RESHAPE variable=a1 complete
	#pragma HLS ARRAY_RESHAPE variable=a2 complete
	#pragma HLS ARRAY_RESHAPE variable=b1 complete
	#pragma HLS ARRAY_RESHAPE variable=b2 complete
	#pragma HLS ARRAY_RESHAPE variable=s1 complete
	#pragma HLS ARRAY_RESHAPE variable=s2 complete
	#pragma HLS ARRAY_RESHAPE variable=t1 complete
	#pragma HLS ARRAY_RESHAPE variable=t2 complete

	_float ai[dim_m];
	_float bi[dim_m];
	_float ci[dim_m];

	#pragma HLS ARRAY_RESHAPE variable=ai complete
	#pragma HLS ARRAY_RESHAPE variable=bi complete
	#pragma HLS ARRAY_RESHAPE variable=ci complete

	_float_bigacc a, b, c;
	_float_bigacc amp[dim_n];
	#pragma HLS ARRAY_RESHAPE variable=amp complete
	//_float_bignum an, bn, cn;
	_angle     theta, thetadiv;
	_sincos    sn, cs;
	int i, j;

	extern int jacobi_seq[dim_n * (dim_n-1) / 2][2];
	#pragma HLS ARRAY_RESHAPE variable=jacobi_seq complete dim=2

	loop_init:for (int i = 0; i < dim_n; i++) {
		for (int j = 0; j < dim_n; j++) {
		#pragma HLS unroll
			U[i][j] = (i==j) ? 1 : 0;
		}
	}

	loop_iter:for (int k = 0; k < maxiter; k++) {
		//iter_stop = true;
		loop_pair:for (int t = 0; t < dim_n * (dim_n-1) / 2; t++){
		#pragma HLS pipeline II=2
		#pragma HLS dependence variable=A inter false
		#pragma HLS dependence variable=U inter false
			i = jacobi_seq[t][0];
			j = jacobi_seq[t][1];
			copy_a:for (int p = 0; p < dim_m; p++) {
			#pragma HLS unroll
				a1[p] = A[i][p];
				a2[p] = A[j][p];
			}
			copy_s:for (int p = 0; p < dim_n; p++) {
			#pragma HLS unroll
				s1[p] = U[i][p];
				s2[p] = U[j][p];
			}

			get_corr_multi:for (int p = 0; p < dim_m; p++) {
			#pragma HLS unroll
				ai[p] = a1[p] * a1[p];
				bi[p] = a2[p] * a2[p];
				ci[p] = a1[p] * a2[p];
			}

			a = 0;
			b = 0;
			c = 0;
			get_corr_add:for (int p = 0; p < dim_m; p++) {
			#pragma HLS unroll
				a += ai[p];
				b += bi[p];
				c += ci[p];
			}
			amp[i] = a;
			amp[j] = b;
//			if (c > offdiag_thres) {
//				iter_stop = false;
//			}

//				an = a;
//				bn = b;
//				cn = c * 2;
//				theta = hls::atan2<16, 9>(cn, bn-an);
//				c = c * 2;A
			theta = hls::atan2(_float_smlacc(c*2), _float_smlacc(b - a));
			thetadiv = theta / 2;
//			if (a < b) {
//				thetadiv += _angle(M_PI_2);
//			}
			thetadiv += _angle(M_PI_2);
			hls::sincos(thetadiv, &sn, &cs);

			ud_a:for (int p = 0; p < dim_m; p++) {
			#pragma HLS unroll
			//#pragma HLS latency min=10
				b1[p] = cs * a1[p] - sn * a2[p];
				b2[p] = sn * a1[p] + cs * a2[p];
			}
			for (int p = 0; p < dim_m; p++) {
			#pragma HLS unroll
				A[i][p] = b1[p];
			}
			for (int p = 0; p < dim_m; p++) {
			#pragma HLS unroll
				A[j][p] = b2[p];
			}

			ud_s:for (int p = 0; p < dim_n; p++) {
			#pragma HLS unroll
			//#pragma HLS latency min=10
				t1[p] = cs * s1[p] - sn * s2[p];
				t2[p] = sn * s1[p] + cs * s2[p];
			}
			for (int p = 0; p < dim_n; p++) {
			#pragma HLS unroll
				U[i][p] = t1[p];
			}
			for (int p = 0; p < dim_n; p++) {
			#pragma HLS unroll
				U[j][p] = t2[p];
			}
		}
//		if (iter_stop) {
//			break;
//		}
	}
}
