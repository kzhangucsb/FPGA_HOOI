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
	const _float ten[numel_outer][dim][numel_inner],
	const _float mat[dim][dim2],
	_float res[numel_outer][dim2][numel_inner]
	) {
	// tensor-matrix multiply along dim-th dimension.
	// ten:     pointer to the tensor. In shape numel_inner * dim * numel_outer.
	// mat:     pointer to the matrix. Should be in shape dim2*dim
	// res:  space to store multiplication result.In shape numel_inner * dim2 * numel_outer.
	#pragma HLS ARRAY_RESHAPE   variable=mat complete dim=2
	#pragma HLS ARRAY_RESHAPE   variable=ten complete dim=3
	#pragma HLS ARRAY_RESHAPE   variable=res complete dim=3
	#pragma HLS RESOURCE variable=mat core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ten core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=res core=RAM_1P_BRAM
	//#pragma HLS INTERFACE m_axi depth=4 port=ten //2097152
	//#pragma HLS INTERFACE m_axi depth=4 port=res

	//#pragma HLS INTERFACE m_axi depth=256 port=ten

	//short offset_o, offset_r;
	_float res_part[numel_inner_batch][dim2];
	#pragma HLS ARRAY_RESHAPE variable=res_part dim=0 complete
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
		loop_interblock:for (int ib = 0; ib < numel_inner / numel_inner_batch; ib += 1) {
		//#pragma HLS pipeline II=30
		//#pragma HLS dataflow
			tmpmemrst:for (int i = 0; i < numel_inner_batch; i++) {
			#pragma HLS UNROLL
				for (int t = 0; t < dim2; t++) {
				#pragma HLS UNROLL
					res_part[i][t] = 0;
				}
			}
			loop_dim:for (int k = 0; k < dim; k++) {
				#pragma HLS pipeline II=1
				loop_interoffset:for (int io = 0; io < numel_inner_batch; io++) {
				#pragma HLS UNROLL
					ten_tmp = ten[j][k][io + ib * numel_inner_batch];
					for (int t = 0; t < dim2; t++) {
					#pragma HLS UNROLL
						res_part[io][t]
							+= ten_tmp * mat[k][t];
					}
				}
			}
			loop_memcpy:for (int t = 0; t < dim2; t++) {
			//#pragma HLS pipeline
				//int base = ib * numel_inner_batch + t * numel_inner + j * numel_inner * dim2;

				for (int io = 0; io < numel_inner_batch; io++) {
				#pragma HLS UNROLL //factor=8
				//#pragma HLS pipeline
					res[j][t][io + ib * numel_inner_batch] =
						res_part[io][t];
				}
			}
		}

	}

}

