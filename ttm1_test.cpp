
#include "type.h"
#include <iostream>
using namespace std;


void ttm(
	const _float ten[128*128*128],
	const _float mat[128*8],
	_float res[128*8*128],
	int numel_inner,
	int numel_outer,
	int dim
	);

int main() {
	int dim = 128;
	//int dim2 = 8;
	int numel_inner = 128;
	int numel_outer = 128;

	int n1 = numel_inner * dim * numel_outer;
	int n2 = dim2*dim;
	int n3 = numel_inner * dim2 * numel_outer;
	_float* ten  = new _float[n1];
	_float* mat  = new _float[n2];
	_float* ten2 = new _float[n3];
	float t;
	FILE * f1 = fopen("/home/zkq/Documents/MATLAB/testcode/f1.txt", "rt");
	if (f1 == NULL) {
		cerr << "unable to find f1" << endl;
		return 2;
	}
	for (int i = 0; i < n1; i++) {
		fscanf(f1, "%f", &t);
		ten[i] = t;
	}
	fclose(f1);

	FILE * f2 = fopen("/home/zkq/Documents/MATLAB/testcode/f2.txt", "rt");
	if (f2 == NULL) {
		cerr << "unable to find f2" << endl;
		return 2;
	}
	for (int i = 0; i < n2; i++) {
		fscanf(f2, "%f", &t);
		mat[i] = t;
	}
	fclose(f2);

	ttm(ten, mat, ten2, numel_inner/32, numel_outer, dim);

	FILE * fo = fopen("/home/zkq/Documents/MATLAB/testcode/fo1.txt", "wt");
	if (fo == NULL) {
		cerr << "unable to create fo1" << endl;
		return 2;
	}
	for (int i = 0; i < n3; i++) {
		fprintf(fo, "%f ", float(ten2[i]));
	}
	fclose(fo);
	printf("Complete!\n");
	delete []ten;
	delete []mat;
	delete []ten2;

	return 0;
}
