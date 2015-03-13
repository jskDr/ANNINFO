#include "fann.h"

/*
Edited by (James) Sung-Jin Kim, jamessungjin.kim@gmail.com. 2015-3-13

From the demo in http://leenissen.dk/fann/html/files2/gettingstarted-txt.html,
the following poitns are updated.
const float desired_error = (const float) 0.000000001;
	const unsigned int max_epochs = 5000000;
*/

int main()
{
	const unsigned int num_input = 2;
	const unsigned int num_output = 1;
	const unsigned int num_layers = 3;

	const unsigned int num_neurons_hidden = 3;
	const float desired_error = (const float) 0.000000001;
	const unsigned int max_epochs = 5000000;
	const unsigned int epoch_between_reports = 1000;

	struct fann *ann = fann_create_standard( num_layers, num_input, num_neurons_hidden, num_output);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file( ann, "xor.data", max_epochs, epoch_between_reports, desired_error);

	fann_save( ann, "xor_float.net");

	fann_destroy( ann);
}
