#include "type.h"
#include <iostream>
#define tensize1 tensize
#define tensize2 tensize
#define tensize3 tensize

using namespace std;

void hooi(
	const _float ten[tensize1*tensize2*tensize3],
	_float coeffmat1[tensize1*rank],
	_float coeffmat2[tensize2*rank],
	_float coeffmat3[tensize3*rank],
	_float core[rank*rank*rank]
);

int main() {

	_float* ten  = new _float[128*128*128];
	_float* coeffmat1  = new _float[128*8];
	_float* coeffmat2  = new _float[128*8];
	_float* coeffmat3  = new _float[128*8];
	_float* core = new _float[8*8*8];
	float t;
	FILE * f1 = fopen("/home/zkq/Documents/MATLAB/testcode/f5.txt", "rt");
	if (f1 == NULL) {
		cerr << "unable to find f1" << endl;
		return 2;
	}
	for (int i = 0; i < 128*128*128; i++) {
		fscanf(f1, "%f", &t);
		ten[i] = t;
	}
	fclose(f1);

	hooi(ten, coeffmat1, coeffmat2, coeffmat3, core);

	FILE * fo;
	fo = fopen("/home/zkq/Documents/MATLAB/testcode/f6o1.txt", "wt");
	if (fo == NULL) {
		cerr << "unable to create fo1" << endl;
		return 2;
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 128; j++) {
			fprintf(fo, "%f ", float(coeffmat1[i*128+j]));
		}
		fprintf(fo, "\n");
	}
	fclose(fo);
	fo = fopen("/home/zkq/Documents/MATLAB/testcode/f6o2.txt", "wt");
	if (fo == NULL) {
		cerr << "unable to create fo2" << endl;
		return 2;
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 128; j++) {
			fprintf(fo, "%f ", float(coeffmat2[i*128+j]));
		}
		fprintf(fo, "\n");
	}
	fclose(fo);
	fo = fopen("/home/zkq/Documents/MATLAB/testcode/f6o3.txt", "wt");
	if (fo == NULL) {
		cerr << "unable to create fo3" << endl;
		return 2;
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 128; j++) {
			fprintf(fo, "%f ", float(coeffmat3[i*128+j]));
		}
		fprintf(fo, "\n");
	}
	fclose(fo);

	fo = fopen("/home/zkq/Documents/MATLAB/testcode/f6o4.txt", "wt");
	if (fo == NULL) {
		cerr << "unable to create fo4" << endl;
		return 2;
	}
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 8; j++) {
			fprintf(fo, "%f ", float(core[i*8+j]));
		}
		fprintf(fo, "\n");
	}
	fclose(fo);

	printf("Complete!\n");
	delete []ten;
	delete []coeffmat1;
	delete []coeffmat2;
	delete []coeffmat3;
	delete []core;

	return 0;
}
