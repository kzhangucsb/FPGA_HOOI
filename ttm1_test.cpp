
#include "type.h"
#include <iostream>
using namespace std;


void ttm(
	_float ten[numel_inner*numel_outer*dim],
	_float mat[dim*dim2],
	_float res[numel_inner*numel_outer*dim2]
	) ;

int main() {

	int n1 = numel_inner * dim * numel_outer;
	int n2 = dim2*dim;
	int n3 = numel_inner * dim2 * numel_outer;
	_float* ten  = new _float[n1];
	_float* mat  = new _float[n2];
	_float* ten2 = new _float[n3];

	FILE * f1 = fopen("/home/zkq/Documents/MATLAB/testcode/f1.txt", "rt");
	if (f1 == NULL) {
		cerr << "unable to find f1" << endl;
		return 1;
	}
	for (int i = 0; i < n1; i++) {
		fscanf(f1, "%f", ten + i);
	}
	fclose(f1);

	FILE * f2 = fopen("/home/zkq/Documents/MATLAB/testcode/f2.txt", "rt");
	if (f2 == NULL) {
		cerr << "unable to find f2" << endl;
		return 1;
	}
	for (int i = 0; i < n2; i++) {
		fscanf(f2, "%f", mat + i);
	}
	fclose(f2);

	ttm(ten, mat, ten2);

	FILE * fo = fopen("/home/zkq/Documents/MATLAB/testcode/fo1.txt", "wt");
	if (fo == NULL) {
		cerr << "unable to create fo1" << endl;
		return 1;
	}
	for (int i = 0; i < n3; i++) {
		fprintf(fo, "%f ", *(ten2 + i));
	}
	fclose(fo);
	printf("Complete!\n");
	delete []ten;
	delete []mat;
	delete []ten2;

	return 0;
}
