//============================================================================
// Name        : test1.cpp
// Author      : zkq
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include "ap_mem_if.h"

#include "type.h"


#define numel_inner_batch 32

void ttm(
	const _float ten[128*128*128],
	const _float mat[128*8],
	_float res[128*8*128],
	int numel_inner_scale, int numel_outer, int dim
	){
	// tensor-matrix multiply along dim-th dimension.
	// ten:     pointer to the tensor. In shape numel_inner * dim * numel_outer.
	// mat:     pointer to the matrix. Should be in shape dim2*dim
	// res:  space to store multiplication result.In shape numel_inner * dim2 * numel_outer.
	#pragma HLS ARRAY_RESHAPE   variable=mat cyclic factor=8
	#pragma HLS ARRAY_RESHAPE   variable=ten cyclic factor=32
	#pragma HLS ARRAY_RESHAPE   variable=res cyclic factor=32
	#pragma HLS RESOURCE variable=mat core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ten core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=res core=RAM_1P_BRAM

	//short offset_o, offset_r;
	int numel_inner = numel_inner_scale * numel_inner_batch;
	_float res_tmp[numel_inner_batch*dim2];


	#pragma HLS ARRAY_RESHAPE variable=res_tmp dim=0 complete
	_float ten_tmp;
	/*
	for (int j = 0; j < numel_outer; j++) {
		offset_r = j * numel_inner * dim2;
		for (int t = 0; t < dim2; t++) {
			for (int i = 0; i < numel_inner; i++) {
				res[i + t * numel_inner + offset_r] = 0;
			}
		}
	}
	*/


	loop_outer:for (int j = 0; j < numel_outer; j++) {
		loop_interblock:for (int ib = 0; ib < numel_inner; ib+=numel_inner_batch) {
			tmpmemrst:for (int i = 0; i < numel_inner_batch; i++) {
			#pragma HLS UNROLL
				for (int t = 0; t < dim2; t++) {
				#pragma HLS UNROLL
					res_tmp[i*dim2+t] = 0;
				}
			}
			loop_dim:for (int k = 0; k < dim; k++) {
				#pragma HLS pipeline II=1
				loop_interoffset:for (int io = 0; io < numel_inner_batch; io++) {
				//#pragma HLS UNROLL factor=8
					ten_tmp = ten[io + ib + k*numel_inner + j*numel_inner*dim];
					for (int t = 0; t < dim2; t++) {
					#pragma HLS UNROLL
						res_tmp[io*dim2+t]
							+= ten_tmp * mat[k*dim2 + t];
					}
				}
			}
			loop_memcpy:for (int t = 0; t < dim2; t++) {
			#pragma HLS pipeline
				for (int io = 0; io < numel_inner_batch; io++) {
				#pragma HLS UNROLL //factor=8
				//#pragma HLS pipeline
					res[io + ib + t * numel_inner + j * numel_inner * dim2] =
						res_tmp[io*dim2+t];
				}
			}
		}

	}
	}

