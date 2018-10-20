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
	const _float ten[numel_inner*numel_outer*dim],
	const _float mat[dim*dim2],
	_float res[numel_inner*numel_outer*dim2]
	) {
	// tensor-matrix multiply along dim-th dimension.
	// ten:     pointer to the tensor. In shape numel_inner * dim * numel_outer.
	// mat:     pointer to the matrix. Should be in shape dim2*dim
	// res:  space to store multiplication result.In shape numel_inner * dim2 * numel_outer.
	#pragma HLS ARRAY_RESHAPE   variable=mat cyclic factor=40
	#pragma HLS ARRAY_RESHAPE   variable=ten cyclic factor=8 dim=0
	#pragma HLS ARRAY_RESHAPE   variable=res cyclic factor=8 dim=0
	#pragma HLS RESOURCE variable=mat core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ten core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=res core=RAM_1P_BRAM

	//short offset_o, offset_r;
	_float res_tmp[numel_inner_batch][dim2];
	#pragma HLS array_reshape variable=res_tmp complete dim=0
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
					res_tmp[i][t] = 0;
				}
			}
			loop_dim:for (int k = 0; k < dim; k++) {
			//#pragma HLS pipeline
				//offset_o = j * numel_inner * dim + k * numel_inner;
				//offset_r = j * numel_inner * dim2;

				loop_interoffset:for (int io = 0; io < numel_inner_batch; io++) {
				#pragma HLS UNROLL factor=8
					for (int t = 0; t < dim2; t++) {
					#pragma HLS UNROLL
						res_tmp[io][t]
							+= ten[io + ib + j*numel_inner*dim + k*numel_inner] * mat[k*dim2 + t];
					}
				}
			}
			loop_memcpy:for (int t = 0; t < dim2; t++) {
			#pragma HLS pipeline
				for (int io = 0; io < numel_inner_batch; io++) {
				#pragma HLS UNROLL factor=8
					res[io + ib + t * numel_inner + j * numel_inner * dim2] =
						res_tmp[io][t];
				}
			}
		}

	}

}

