#include "type.h"
#define tensize1 tensize
#define tensize2 tensize
#define tensize3 tensize


#define niter 8

#define dim_n 128
#define dim_m 64

void svd(_float A[dim_n][dim_m], _float U[dim_n][dim_n]);
void ttm(const _float ten[], const _float mat[], _float res[],
	int numel_inner_scale, int numel_outer, int dim);
void ttma1(_float ten[], _float mat[], _float res[], int nrow);

void hooi(
	const _float ten[tensize1*tensize2*tensize3],
	_float coeffmat1[tensize1*rank],
	_float coeffmat2[tensize2*rank],
	_float coeffmat3[tensize3*rank],
	_float core[rank*rank*rank]
){

#pragma HLS ARRAY_RESHAPE   variable=ten cyclic factor=32
#pragma HLS ARRAY_RESHAPE   variable=coeffmat1 cyclic factor=32
#pragma HLS ARRAY_RESHAPE   variable=coeffmat2 cyclic factor=32
#pragma HLS ARRAY_RESHAPE   variable=coeffmat3 cyclic factor=32
#pragma HLS ARRAY_RESHAPE   variable=core      cyclic factor=32
#pragma HLS array_map       variable=coeffmat1 instance=res
#pragma HLS array_map       variable=coeffmat2 instance=res
#pragma HLS array_map       variable=coeffmat3 instance=res
#pragma HLS array_map       variable=core      instance=res
//#pragma HLS INTERFACE m_axi depth=65536        port=ten
//#pragma HLS INTERFACE m_axi depth=400          port=coeffmat1
//#pragma HLS INTERFACE m_axi depth=400          port=coeffmat2
//#pragma HLS INTERFACE m_axi depth=400          port=coeffmat3
//#pragma HLS INTERFACE m_axi depth=400          port=core

	_float tentmp1[tensize1*tensize2*rank], tentmp2[tensize1*rank*rank];
#pragma HLS ARRAY_RESHAPE   variable=tentmp1 cyclic factor=32
#pragma HLS ARRAY_RESHAPE   variable=tentmp2 cyclic factor=32
	_float svd_u[dim_n][dim_n], svd_a[dim_n][dim_m];
#pragma HLS ARRAY_RESHAPE   variable=svd_u complete dim=2
#pragma HLS ARRAY_RESHAPE   variable=svd_u complete  dim=2
	_float coefftmp1[tensize1*rank], coefftmp2[rank*tensize2], coefftmp3[rank*tensize3];
#pragma HLS ARRAY_RESHAPE   variable=coefftmp1 cyclic factor=32
#pragma HLS ARRAY_RESHAPE   variable=coefftmp2 cyclic factor=8
#pragma HLS ARRAY_RESHAPE   variable=coefftmp3 cyclic factor=8
	// initialization
	for (int m = 0; m < rank; m++){
		for (int n = 0; n < tensize1; n++) {
			coefftmp1[m*tensize1+n] = (m == n) ? 1 : 0;
		}
	}
	for (int m = 0; m < tensize2; m++){
		for (int n = 0; n < rank; n++) {
			coefftmp2[m*rank+n] = (m == n) ? 1 : 0;
		}
	}
	for (int m = 0; m < tensize3; m++){
		for (int n = 0; n < rank; n++) {
			coefftmp3[m*rank+n] = (m == n) ? 1 : 0;
		}
	}
#pragma HLS allocation instances=ttm limit=1 function
#pragma HLS allocation instances=ttma1 limit=1 function
#pragma HLS allocation instances=svd limit=1 function

	for (int iit = 0; iit < niter; iit++) {
		// update first coefficient matrix
		ttm(ten,     coefftmp3, tentmp1, tensize1*tensize2/ttmbatsize, 1, tensize3);
		ttm(tentmp1, coefftmp2, tentmp2, tensize1/ttmbatsize, rank, tensize2);
		for (int m = 0; m < tensize1; m++) {
			for (int n = 0; n < rank*rank; n++) {
				svd_a[m][n] = tentmp2[n*tensize1+m];
			}
		}
		svd(svd_a, svd_u);
		for (int m = 0; m < rank; m++) {
			for (int n = 0; n < tensize1; n++) {
				coefftmp1[m*tensize1+n] = svd_u[m][n];
			}
		}

		//update second coefficient matrix
		ttm  (ten,     coefftmp3, tentmp1, tensize1*tensize2/ttmbatsize, 1, tensize3);
		ttma1(tentmp1, coefftmp1, tentmp2, tensize2*rank);
		for (int k = 0; k < rank; k++) {
			for (int m = 0; m < tensize2; m++) {
				for (int n = 0; n < rank; n++) {
				//#pragma HLS UNROLL
					svd_a[m][n+k*rank] = tentmp2[n+m*rank+k*rank*tensize2];
				}
			}
		}
		svd(svd_a, svd_u);
		for (int m = 0; m < rank; m++) {
			for (int n = 0; n < tensize1; n++) {
				coefftmp2[m+n*rank] = svd_u[m][n];
			}
		}
		//update third coefficient matrix
		ttm  (ten,     coefftmp2, tentmp1, tensize1/ttmbatsize, tensize3, tensize2);
		ttma1(tentmp1, coefftmp1, tentmp2, tensize3*rank);
		for (int m = 0; m < tensize3; m++) {
			for (int n = 0; n < rank*rank; n++) {
			//#pragma HLS UNROLL factor=32
				svd_a[m][n] = tentmp2[n+m*rank*rank];
			}
		}
		svd(svd_a, svd_u);
		for (int m = 0; m < rank; m++) {
			for (int n = 0; n < tensize1; n++) {
				coefftmp3[m+n*rank] = svd_u[m][n];
			}
		}
	}
	// calculate core tensor
	ttm  (ten,     coefftmp3, tentmp1, tensize1*tensize2/ttmbatsize, 1, tensize3);
	ttm  (tentmp1, coefftmp2, tentmp2, tensize1/ttmbatsize, rank, tensize2);
	ttma1(tentmp2, coefftmp1, core   , rank*rank);

	// transpose to get coefficient matrix
	for (int m = 0; m < rank; m++){
		for (int n = 0; n < tensize1; n++) {
		//#pragma HLS UNROLL factor=32
			coeffmat1[m*tensize1+n] = coefftmp1[m*tensize1+n];
		}
	}
	for (int m = 0; m < rank; m++){
		for (int n = 0; n < tensize1; n++) {
			coeffmat2[m*tensize1+n] = coefftmp2[m+n*rank];
		}
	}
	for (int m = 0; m < rank; m++){
		for (int n = 0; n < tensize1; n++) {
			coeffmat3[m*tensize1+n] = coefftmp3[m+n*rank];
		}
	}
}

void hooi_top(
	const _float ten[tensize1*tensize2*tensize3],
	_float res[3584]
) {
#pragma HLS ARRAY_RESHAPE   variable=ten cyclic factor=32
#pragma HLS ARRAY_RESHAPE   variable=res cyclic factor=32
#pragma HLS INTERFACE m_axi depth=65536        port=ten
#pragma HLS INTERFACE m_axi depth=112          port=res
	hooi(ten, res, res+1024, res+2048, res+3072);
}
