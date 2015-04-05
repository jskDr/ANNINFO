#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fann.h"

/*
Edited by (James) Sung-Jin Kim, jamessungjin.kim@gmail.com. 2015-3-13 ~ 2015-4-5

This codes refer to the demo codes in http://leenissen.dk/fann/html/files2/gettingstarted-txt.html,
Especially, xor_train() is modified version of the demo code there. 

Aditionally, machine learnig code for molecules QSPR/QSAR are added 
as the name of aq_X, aq_n_X and aq_n_io_X, among which ag_n_io_X offers most advanced features.
*/

// ============================================================================================
// XOR Network
// ============================================================================================
int xor_train()
{
  const unsigned int num_input = 2;
  const unsigned int num_output = 1;
  const unsigned int num_layers = 3;

  const unsigned int num_neurons_hidden = 3;
  const float desired_error = (const float) 0.00001;
  const unsigned int max_epochs = 50000;
  const unsigned int epoch_between_reports = 10000;

  struct fann *ann = fann_create_standard( num_layers, num_input, num_neurons_hidden, num_output);

  fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
  
  // For binary output systems, the output filter is used by SIGMOID. 
  fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

  // The first line of input data is consisted of 
  // 1) the number of traning data set which is 
  // a pair of the input patter and the output patter (xor case --> 4),
  // 2) the number of the input pattern elements (xor case --> 2),
  // 3) the number of the output pattern elements (xor case --> 1).
  fann_train_on_file( ann, "xor.data", max_epochs, epoch_between_reports, desired_error);

  fann_save( ann, "xor_float.net");

  fann_destroy( ann);

  return 0;
}

int xor_train_err( float desired_error)
{
  const unsigned int num_input = 2;
  const unsigned int num_output = 1;
  const unsigned int num_layers = 3;

  const unsigned int num_neurons_hidden = 3;
  // const float desired_error = (const float) 0.00001;
  const unsigned int max_epochs = 50000;
  const unsigned int epoch_between_reports = 10000;

  struct fann *ann = fann_create_standard( num_layers, num_input, num_neurons_hidden, num_output);

  fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
  fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

  fann_train_on_file( ann, "xor.data", max_epochs, epoch_between_reports, desired_error);

  fann_save( ann, "xor_float.net");

  // array architecture should be destroid.
  fann_destroy( ann);

  return 0;
}

int xor_run()
{
  fann_type *calc_out;
  fann_type input[2];
  const int Inout[2] = {1, -1}; // This is not changed and used for validation input date
  int i0, i1, c_int;

  // fann can be build from X=standard or X=from_file such as fann_create_X
  struct fann *ann = fann_create_from_file("xor_float.net");

  for( i0 = 0; i0 < 2; i0++) {
    for( i1 = 0; i1 < 2; i1++) {
      input[0] = Inout[ i0];
      input[1] = Inout[ i1];

      calc_out = fann_run( ann, input); // memory for calc_out is allocated in fann_run(), highly probably
      c_int = (int) round(calc_out[0]);

      printf("xor test (%f, %f) -> int(%f) -> %d\n", input[0], input[1], calc_out[0], c_int);
    }
  }

  fann_destroy(ann);

  return 0;
}

int fprintf_header( FILE* fpw, int argc, char *argv[])
{
  int ii;
  for( ii = 0; ii < argc - 1; ii++) {
    fprintf( fpw, "\"%s\",", argv[ ii]);
    printf( "\"%s\",", argv[ ii]);
  }
  if( argc > 0) {
    fprintf( fpw, "\"%s\"\n", argv[ ii]);
    printf( "\"%s\"\n", argv[ ii]);
  }

  return 0;
}


int xor_run_file( char *fname)
{
  fann_type *calc_out;
  fann_type input[2];
  // int inout[2] = {1, -1}, i0, i1, c_int;
  int inout[2] = {1, -1}, i0, i1;

  // A neural network is built on the trained results.
  struct fann *ann = fann_create_from_file("xor_float.net");

  // File is used to save the output;
  FILE *fpw;
  char *header[] = {"in0", "in1", "out"};
  fpw = fopen( fname, "w");
  if(fpw != NULL) {
    fprintf_header( fpw, 3, header);
    // fprintf( fpw, "\"%s\",\"%s\",\"%s\"\n", "in0", "in1", "out");
    // printf( "\"%s\",\"%s\",\"%s\"\n", "in0", "in1", "out");

    for( i0 = 0; i0 < 2; i0++) {
      for( i1 = 0; i1 < 2; i1++) {
        input[0] = inout[ i0];
        input[1] = inout[ i1];

        calc_out = fann_run( ann, input);
        // c_int = (int) round(calc_out[0]);

        fprintf( fpw, "%f,%f,%f\n", input[0], input[1], calc_out[0]);
        printf( "%f,%f,%f\n", input[0], input[1], calc_out[0]);
      }
    }
    
    fclose( fpw);
  }
  else {
    printf("File of (%s) can not be opened.\n", fname);
  }

  fann_destroy(ann);

  return 0;
}


int xor_main()
{
  // xor_train();
  printf("Traninig...\n");
  xor_train();

  printf("Running...\n");
  xor_run_file( "xoroutdata.csv");

  return 0;
}

int _xor_main_err_r0( float desired_error)
{
  xor_train_err( desired_error);
  xor_run();

  return 0;
}

int xor_main_err( float desired_error)
{
  xor_train_err( desired_error);
  xor_run_file( "xoroutdata.csv");

  return 0;
}

// ============================================================================================
// Anthraquinone with 1 ANN input node
// ============================================================================================
#define n_INOUT_N 5
#define n_IN_N 2

int aq_train()
{
  // only aq.data and aq_float.net are applied. 
  // output function is changed to linear for equlization instead of decision.
  const unsigned int num_input = 1;
  const unsigned int num_output = 1;
  const unsigned int num_layers = 3;

  const unsigned int num_neurons_hidden = 2;
  const float desired_error = (const float) 0.0005;
  const unsigned int max_epochs = 50000;
  const unsigned int epoch_between_reports = 10000;

  struct fann *ann = fann_create_standard( num_layers, num_input, num_neurons_hidden, num_output);

  fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
  fann_set_activation_function_output(ann, FANN_LINEAR);

  fann_train_on_file( ann, "aq_1.data", max_epochs, epoch_between_reports, desired_error);

  fann_save( ann, "aq_float.net");

  fann_destroy( ann);

  return 0;
}

int aq_train_err( float desired_error)
{
  // only aq.data and aq_float.net are applied. 
  // output function is changed to linear for equlization instead of decision.
  const unsigned int num_input = 1;
  const unsigned int num_output = 1;
  const unsigned int num_layers = 3;

  const unsigned int num_neurons_hidden = 2;
  // const float desired_error = (const float) 0.0005;
  const unsigned int max_epochs = 50000;
  const unsigned int epoch_between_reports = 10000;

  struct fann *ann = fann_create_standard( num_layers, num_input, num_neurons_hidden, num_output);

  fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
  fann_set_activation_function_output(ann, FANN_LINEAR);

  fann_train_on_file( ann, "aq_1.data", max_epochs, epoch_between_reports, desired_error);

  fann_save( ann, "aq_float.net");

  fann_destroy( ann);

  return 0;
}

#define INOUT_N 5
#define IN_N 1

int _aq_run_r0()
{
  fann_type *calc_out;
  fann_type input[ IN_N];
  float inout[ INOUT_N] = {1.0, 0.67, 0.65, 0.58, 0.27};
  int i0, i1, c_int;

  struct fann *ann = fann_create_from_file("aq_float.net");

  for( i0 = 0; i0 < INOUT_N; i0++) {
    input[0] = inout[ i0];
    // input[1] = 1.0;

    calc_out = fann_run( ann, input);

    printf("test (%f) -> %f\n", input[0], calc_out[0]);
  }

  fann_destroy(ann);

  return 0;
}

int aq_run( )
{
  fann_type *calc_out;
  fann_type input[ IN_N];
  float inout[ INOUT_N] = {1.0, 0.67, 0.65, 0.58, 0.27};
  int i0, i1, c_int;
  float in_val;

  struct fann *ann = fann_create_from_file("aq_float.net");

  for( i0 = 0; i0 < INOUT_N; i0++) {
    input[0] = inout[ i0];
    // input[1] = 1.0;

    calc_out = fann_run( ann, input);

    printf("test (%f) -> %f\n", input[0], calc_out[0]);
  }

  for( in_val = 0.2; in_val < 1.2; in_val += 0.1) {
    calc_out = fann_run( ann, &in_val);

    // printf("test (%f) -> %f\n", in_val, calc_out[0]); 
    printf("%f, ", calc_out[0]);
  }
  printf("\n");

  fann_destroy(ann);

  return 0;
}

int aq_run_file( char* fname)
{
  fann_type *calc_out;
  fann_type input[ IN_N];
  float inout[ INOUT_N] = {1.0, 0.67, 0.65, 0.58, 0.27};
  int i0, i1, c_int;
  float in_val;

  struct fann *ann = fann_create_from_file("aq_float.net");

  FILE *fpw;
  char *header[] = {"in", "out"};

  fpw = fopen( fname, "w");
  if(fpw != NULL) {
    fprintf_header( fpw, 2, header);

    for( i0 = 0; i0 < INOUT_N; i0++) {
      input[0] = inout[ i0];
      // input[1] = 1.0;

      calc_out = fann_run( ann, input);

      fprintf( fpw, "%f, %f\n", input[0], calc_out[0]);
      printf( "%f, %f\n", input[0], calc_out[0]);
    }

    for( in_val = 0.0; in_val <= 1.05; in_val += 0.1) {
      calc_out = fann_run( ann, &in_val);

    // printf("test (%f) -> %f\n", in_val, calc_out[0]); 
      fprintf( fpw, "%f,%f\n", in_val, calc_out[0]);
      printf( "%f,%f\n", in_val, calc_out[0]);
    }

    fclose( fpw);
  }
  else {
    printf("File of (%s) can not be opened.\n", fname);    
  }

  fann_destroy(ann);

  return 0;
}

int _aq_main_r0()
{
  printf("Traninig...\n");
  aq_train();
  
  // aq_run();
  printf("Running...\n");
  aq_run();

  return 0;
}

int _aq_main_err_r0( float desired_error)
{
  aq_train_err( desired_error);
  aq_run();

  return 0;
}

int aq_main()
{
  printf("Traninig...\n");
  aq_train();
  
  // aq_run();
  printf("Running...\n");
  aq_run_file( "aqoutdata.csv");

  return 0;
}

int aq_main_err( float desired_error)
{
  aq_train_err( desired_error);
  aq_run_file( "aqoutdata.csv");

  return 0;
}


// ============================================================================================
// Anthraquinone with N ANN input node, where N=2 now
// ============================================================================================
#define n_INOUT_N 5
#define n_IN_N 2

int aq_n_train( unsigned int num_input)
{
  // only aq.data and aq_float.net are applied. 
  // output function is changed to linear for equlization instead of decision.

  // const float learning_rate = 0.7f;

  // const unsigned int num_input = 1;
  const unsigned int num_output = 1;
  const unsigned int num_layers = 3;

  const unsigned int num_neurons_hidden = 2;
  const float desired_error = (const float) 0.00001;
  const unsigned int max_epochs = 50000;
  const unsigned int epoch_between_reports = 10000;

  struct fann *ann = fann_create_standard( num_layers, num_input, num_neurons_hidden, num_output);

  // fann_set_learning_rate(ann, learning_rate);

  fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
  fann_set_activation_function_output(ann, FANN_LINEAR);

  fann_train_on_file( ann, "aq_2.data", max_epochs, epoch_between_reports, desired_error);

  fann_save( ann, "aq_float.net");

  fann_destroy( ann);

  return 0;
}

int aq_n_train_err( unsigned int num_input, float desired_error)
{
  // only aq.data and aq_float.net are applied. 
  // output function is changed to linear for equlization instead of decision.

  // const float learning_rate = 0.7f;

  // const unsigned int num_input = 1;
  const unsigned int num_output = 1;
  const unsigned int num_layers = 3;

  const unsigned int num_neurons_hidden = 2;
  // const float desired_error = (const float) 0.00001;
  const unsigned int max_epochs = 50000;
  const unsigned int epoch_between_reports = 10000;

  struct fann *ann = fann_create_standard( num_layers, num_input, num_neurons_hidden, num_output);

  // fann_set_learning_rate(ann, learning_rate);

  fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
  fann_set_activation_function_output(ann, FANN_LINEAR);

  fann_train_on_file( ann, "aq_2.data", max_epochs, epoch_between_reports, desired_error);

  fann_save( ann, "aq_float.net");

  fann_destroy( ann);

  return 0;
}


int _aq_n_run_r0()
{
  fann_type *calc_out;
  fann_type input[ n_IN_N];
  float inout[ n_INOUT_N][ n_IN_N] = { {1.0, 0.28}, {0.67, 0.46}, {0.65, 0.41}, {0.58, 0.49}, {0.28, 1}};
  int i0, i1, c_int;
  float in_val;

  struct fann *ann = fann_create_from_file("aq_float.net");

  for( i0 = 0; i0 < n_INOUT_N; i0++) {
    for( i1 = 0; i1 < n_IN_N; i1++) {
      input[ i1] = inout[ i0][ i1];

      printf("input[%d] = %f, ", i1, input[ i1]);
    }
      
    calc_out = fann_run( ann, input);

    printf("output -> %f\n", calc_out[0]);
  }

  fann_destroy(ann);

  return 0;
}

int aq_n_run()
{
  fann_type *calc_out;
  fann_type input[ n_IN_N];
  float inout[ n_INOUT_N][ n_IN_N] = { {1.0, 0.28}, {0.67, 0.46}, {0.65, 0.41}, {0.58, 0.49}, {0.28, 1}};
  int i0, i1, c_int;
  float in_val;

  struct fann *ann = fann_create_from_file("aq_float.net");

  for( i0 = 0; i0 < n_INOUT_N; i0++) {
    for( i1 = 0; i1 < n_IN_N; i1++) {
      input[ i1] = inout[ i0][ i1];

      printf("input[%d] = %f, ", i1, input[ i1]);
    }
      
    calc_out = fann_run( ann, input);

    printf("output -> %f\n", calc_out[0]);
  }

  fann_destroy(ann);

  return 0;
}

int aq_n_run_file( char *fname)
{
  fann_type *calc_out;
  fann_type input[ n_IN_N];
  float inout[ n_INOUT_N][ n_IN_N] = { {1.0, 0.28}, {0.67, 0.46}, {0.65, 0.41}, {0.58, 0.49}, {0.28, 1}};
  int i0, i1, c_int;
  float in_val;

  struct fann *ann = fann_create_from_file("aq_float.net");

  // File is used to save the output;
  FILE *fpw;
  char *header[] = {"in0", "in1", "out"};

  fpw = fopen( fname, "w");
  if(fpw != NULL) {
    fprintf_header( fpw, 3, header);

    for( i0 = 0; i0 < n_INOUT_N; i0++) {
      for( i1 = 0; i1 < n_IN_N; i1++) {
        input[ i1] = inout[ i0][ i1];
        // printf("input[%d] = %f, ", i1, input[ i1]);
      }
  
      calc_out = fann_run( ann, input);
      fprintf( fpw, "%f,%f,%f\n", input[0], input[1], calc_out[0]);
      printf( "%f,%f,%f\n", input[0], input[1], calc_out[0]);
    }

    fclose( fpw);      
  }
  else {
    printf("File of (%s) can not be opened.\n", fname);
  }

  fann_destroy(ann);

  return 0;
}

int aq_n_main()
{
  int num_input = n_IN_N;
  aq_n_train( num_input);
  aq_n_run_file( "aqnoutdata.csv");

  return 0;
}

int aq_n_main_err( float desired_error)
{
  int num_input = n_IN_N;
  aq_n_train_err( num_input, desired_error);
  aq_n_run_file( "aqnoutdata.csv");

  return 0;
}

// ============================================================================================
// N Input Programe
// ============================================================================================
int get_ann_info( char *fname, unsigned int *p_num_input, unsigned int *p_num_output)
{
  FILE *fpr;
  unsigned int tmp;

  fpr = fopen( fname, "r");
  if( fpr != NULL) {
    fscanf( fpr, "%d %d %d", &tmp, p_num_input, p_num_output);
    printf("ANN: num_input=%d, num_output=%d\n", *p_num_input, *p_num_output);
    fclose( fpr);
  }
  else{
    printf( "The input file of (%s) has a problem.\n", fname);
  }

  return 0;
}

int aq_n_train_io( char* indata)
{
  // only aq.data and aq_float.net are applied. 
  // output function is changed to linear for equlization instead of decision.

  // const float learning_rate = 0.7f;

  const unsigned int num_layers = 3;

  const unsigned int num_neurons_hidden = 2;
  const float desired_error = (const float) 0.00001;
  const unsigned int max_epochs = 50000;
  const unsigned int epoch_between_reports = 10000;

  unsigned int num_output;
  unsigned int num_input;

  get_ann_info( indata, &num_input, &num_output);

  struct fann *ann = fann_create_standard( num_layers, num_input, num_neurons_hidden, num_output);

  // fann_set_learning_rate(ann, learning_rate);

  fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
  fann_set_activation_function_output(ann, FANN_LINEAR);

  fann_train_on_file( ann, indata, max_epochs, epoch_between_reports, desired_error);

  fann_save( ann, "aq_float.net");

  fann_destroy( ann);

  return 0;
}

struct st_inout {
  float **data;
  unsigned int out_n;
  unsigned int in_n;
};

int show_vec_float( float* f, const unsigned int L) {
  unsigned int l;
  for( l = 0; l < L - 1; l++) {
    printf( "%f, ", f[l]);
  }
  printf( "%f\n", f[L-1]);

  return 0;
}

FILE* get_inout(struct st_inout *pinout, char *fdata) {
  FILE *fpr;
  unsigned int in_n, out_n;

  fpr = fopen( fdata, "r");
  if( fpr != NULL) {
    fscanf( fpr, "%d %d", &pinout->out_n, &pinout->in_n);
    printf( "OUT_N=%d, IN_N=%d\n", pinout->out_n, pinout->in_n);

    pinout->data = (float**) malloc( pinout->out_n * sizeof(float*)); // <-- it should be deleted 
    for( out_n = 0; out_n < pinout->out_n; out_n++){
      pinout->data[out_n] = (float*) malloc( pinout->in_n * sizeof(float)); // <-- it should be deleted.
      for( in_n = 0; in_n < pinout->in_n; in_n++){
        fscanf( fpr, "%f", &pinout->data[out_n][in_n]);
        // printf( "pinout->data[%d][%d] = %f\n", out_n, in_n, pinout->data[out_n][in_n])
        // printf( "%f, ", pinout->data[out_n][in_n]);
      }
      show_vec_float( pinout->data[out_n], pinout->in_n);
    }
    
    fclose( fpr);
  }
  else {
    printf("No run.data is working (non-exist or has problems)\n");
  }

  return fpr;
}

int close_inout(struct st_inout *pinout) {
  unsigned int out_n;
  for( out_n = 0; out_n < pinout->out_n; out_n++){
    free( pinout->data[out_n]);
  }
  free( pinout->data);

  return 0;
}

int test_get_inout()
{
  FILE *pt;
  struct st_inout inout;
  pt = get_inout( &inout, "run.data");
  if( pt != NULL) {
    close_inout( &inout);
  }

  return 0;
}

int aq_n_run_file_io( char *fname, char *fdata)
{
  fann_type *calc_out;
  fann_type input[ n_IN_N];
  //float inout[ n_INOUT_N][ n_IN_N] = { {1.0, 0.28}, {0.67, 0.46}, {0.65, 0.41}, {0.58, 0.49}, {0.28, 1}};
  
  struct st_inout inout;
  FILE *pt;
  
  int i0, i1, c_int;
  float in_val;

  struct fann *ann = fann_create_from_file("aq_float.net");

  // File is used to save the output;
  FILE *fpw;
  char *header[] = {"in0", "in1", "out"};

  pt = get_inout( &inout, fdata);
  if( pt != NULL) {
    fpw = fopen( fname, "w");
    if(fpw != NULL) {
      fprintf_header( fpw, 3, header);

      for( i0 = 0; i0 < inout.out_n; i0++) {
        for( i1 = 0; i1 < inout.in_n; i1++) {
          input[ i1] = inout.data[ i0][ i1];
          // printf("input[%d] = %f, ", i1, input[ i1]);
        }
    
        calc_out = fann_run( ann, input);
        for( i1 = 0; i1 < inout.in_n; i1++) {
          fprintf( fpw, "%f,", input[ i1]);
          printf( "%f,", input[ i1]);
        }
        fprintf( fpw, "%f\n", calc_out[0]);
        printf( "%f\n", calc_out[0]);
      }

      fclose( fpw);      
    }
    else {
      printf("File of (%s) can not be opened.\n", fname);
    }

    close_inout( &inout);
  }
  else {
    printf("Run data of (%s) is not valid.\n", fdata);
  }

  fann_destroy(ann);

  return 0;
}

int aq_n_io_main()
{
  int num_input = n_IN_N;
  aq_n_train_io( "aq_2.data");
  aq_n_run_file_io( "aqnoutdata.csv", "aq_2_run.data");

  return 0;
}

// ============================================================================================
// Main programe
// ============================================================================================
int show_help()
{
    printf("Usage-1: ann_aq mode\n");
    printf("Usage-2: ann_aq mode param\n");
    printf("Help: ann_aq\n");
    printf("Where\n");
    printf("  mode = xor, aq, aq_n\n");
    printf("  param = desired_error\n");

    return 0;
}

int test_main()
{
  test_get_inout();

  return 0;
}

int run_main(int argc, char* argv[])
{
  float desired_error;

  if( argc == 2) {
    printf("Run: ann_aq %s\n", argv[1]);
    if( !strcmp( argv[1], "xor")) { 
      printf("xor\n");
      xor_main();
    }
    else if( !strcmp( argv[1], "aq")) { 
      printf("aq_main\n");
      aq_main();
    }
    else if( !strcmp( argv[1], "aq_n")) { 
      printf("aq_n_main\n");
      aq_n_main();
    }
    else if( !strcmp( argv[1], "aq_n_io")) { 
      printf("aq_n_io_main\n");
      aq_n_io_main();
    }
    else {
      show_help();
    }
  }
  else if( argc == 3) {
    desired_error = atof( argv[2]);
    printf("Run: ann_aq %s %f\n", argv[1], desired_error);
    if( !strcmp( argv[1], "xor")) { xor_main_err( desired_error);}
    else if( !strcmp( argv[1], "aq")) { aq_main_err( desired_error);}
    else if( !strcmp( argv[1], "aq_n")) { aq_n_main_err( desired_error);}
    else {
      show_help();
    }
  }
  else {
    show_help();
  }

  return 0;
}

int main( int argc, char* argv[])
{
  // test_main();
  run_main( argc, argv);
}