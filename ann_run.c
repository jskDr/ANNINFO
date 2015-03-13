#include <stdio.h>
#include <math.h>
#include "floatfann.h"

int main()
{
	fann_type *calc_out;
	fann_type input[2];
	int inout[2] = {1, -1}, i0, i1, c_int;

	struct fann *ann = fann_create_from_file("xor_float.net");

	for( i0 = 0; i0 < 2; i0++) {
		for( i1 = 0; i1 < 2; i1++) {
			input[0] = inout[ i0];
			input[1] = inout[ i1];

			calc_out = fann_run( ann, input);
			c_int = (int) round(calc_out[0]);

			printf("xor test (%f, %f) -> int(%f) -> %d\n", input[0], input[1], calc_out[0], c_int);
		}
	}

	fann_destroy(ann);

	return 0;
}
