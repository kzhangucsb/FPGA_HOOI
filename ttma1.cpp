#include "type.h"
#define nrowmax tensize*rank*rank
#define dim tensize
#define dim2 rank

void ttma1(_float ten[nrowmax*dim], _float mat[dim*dim2], _float res[nrowmax*dim2], int nrow) {
	#pragma HLS ARRAY_RESHAPE   variable=mat cyclic factor=32
	#pragma HLS ARRAY_RESHAPE   variable=ten cyclic factor=32
	#pragma HLS ARRAY_RESHAPE   variable=res cyclic factor=32
	#pragma HLS RESOURCE variable=mat core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ten core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=res core=RAM_1P_BRAM

	_float cast_in_a, cast_in_b, mult, sum_mult;
	a_row_loop: for (int r = 0; r < nrow; r++) {
		b_col_loop: for (int c = 0; c < dim2; c++) {
			a_col_loop: for (int k = 0; k < dim; k++) {
				#pragma HLS PIPELINE II=1
				#pragma HLS UNROLL factor=32
				cast_in_a = ten[r*dim+k];
			    cast_in_b = mat[c*dim+k];
			    mult      = cast_in_a * cast_in_b;

			    // Cast mult to the correct output size before adding.

			    if (k==0)
			    	sum_mult = mult;
			    else
			    	sum_mult += mult;

			    // Store result
			    if (k == dim-1) {
			    	res[r*dim2+c] = sum_mult;
			    }
			}
		}
	}
}
