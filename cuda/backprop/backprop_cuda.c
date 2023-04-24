// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cuda.h>

#include "util.h"
#include "backprop.h"
#include "cca_benchmark.h"

struct timestamp ts_init, ts_total, ts_memalloc, ts_h2d, ts_d2h, ts_kernel, ts_close;
float init_time = 0, mem_alloc_time = 0, h2d_time = 0, kernel_time = 0,
      d2h_time = 0, close_time = 0, total_time = 0, d2h_phy_time = 0, h2d_phy_time = 0;

////////////////////////////////////////////////////////////////////////////////

void bpnn_layerforward(float *l1, float *l2, float **conn, int n1, int n2);

void bpnn_output_error(float *delta, float *target, float *output, int nj, float *err);

void bpnn_hidden_error(float *delta_h, int nh, float *delta_o, int no, float **who, float *hidden, float *err);

void bpnn_adjust_weights(float *delta, int ndelta, float *ly, int nly, float **w, float **oldw);

float **alloc_2d_dbl(int m, int n);

float squash(float x);

unsigned int num_threads = 0;
unsigned int num_blocks = 0;

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	CCA_BENCHMARK_INIT;
	setup(argc, argv);

	return 0;
}

CUresult bpnn_layerforward_launch
(CUmodule mod, CUdeviceptr input_cuda, CUdeviceptr output_hidden_cuda,
 CUdeviceptr input_hidden_cuda, CUdeviceptr hidden_partial_sum,
 int in, int hid) 
{
	int bdx, bdy, gdx, gdy;
	void* param[] = {&input_cuda, &output_hidden_cuda, &input_hidden_cuda,
					 &hidden_partial_sum, &in, &hid, NULL};
	CUfunction f;
	CUresult res;

	bdx = 16;
	bdy = 16;
	gdx = 1;
	gdy = num_blocks;

	/* get functions. */
	res = cuModuleGetFunction(&f, mod, "_Z22bpnn_layerforward_CUDAPfS_S_S_ii");
	if (res != CUDA_SUCCESS) {
		printf("cuModuleGetFunction(layerforward) failed: res = %u\n", res);
		return res;
	}

	res = cuLaunchKernel(f, gdx, gdy, 1, bdx, bdy, 1, 0, 0, (void**) param, 0);
	if (res != CUDA_SUCCESS) {
		printf("cuLaunchKernel(layerforward) failed: res = %u\n", res);
		return res;
	}

	return CUDA_SUCCESS;
}

/* int is 64-bit for some reason... */
CUresult bpnn_adjust_weights_launch
(CUmodule mod, CUdeviceptr delta, long hid, CUdeviceptr ly, long in,          
 CUdeviceptr w, CUdeviceptr oldw) 
{
	int bdx, bdy, gdx, gdy;
	void* param[] = {&delta, &hid, &ly, &in, &w, &oldw, NULL};
	CUfunction f;
	CUresult res;

	bdx = 16;
	bdy = 16;
	gdx = 1;
	gdy = num_blocks;

	/* get functions. */
	res = cuModuleGetFunction(&f, mod, "_Z24bpnn_adjust_weights_cudaPfiS_iS_S_");
	if (res != CUDA_SUCCESS) {
		printf("cuModuleGetFunction(adjust_weights) failed: res = %u\n", res);
		return res;
	}

	res = cuLaunchKernel(f, gdx, gdy, 1, bdx, bdy, 1, 0, 0, (void**) param, 0);
	if (res != CUDA_SUCCESS) {
		printf("cuLaunchKernel(adjust_weights) failed: res = %u\n", res);
		return res;
	}

	return CUDA_SUCCESS;
}

void bpnn_train_cuda(BPNN *net, float *eo, float *eh)
{
	int j, k;
	int in, hid, out;
	float out_err, hid_err;

	in = net->input_n;
	hid = net->hidden_n;
	out = net->output_n;   

#ifdef GPU  
	int m = 0;
	float *partial_sum;
	float sum;
	float *input_weights_one_dim;
	float *input_weights_prev_one_dim;
	num_blocks = in / 16;  
	CUdeviceptr input_cuda;
	CUdeviceptr input_hidden_cuda;
	CUdeviceptr output_hidden_cuda;
	CUdeviceptr hidden_partial_sum;
	CUdeviceptr hidden_delta_cuda;
	CUdeviceptr input_prev_weights_cuda;
	CUcontext ctx;
	CUmodule mod;
	CUresult res;

	input_weights_one_dim = 
		(float *) malloc((in + 1) * (hid + 1) * sizeof(float));
	input_weights_prev_one_dim = 
		(float *) malloc((in + 1) * (hid + 1) * sizeof(float));
	partial_sum = (float *) malloc(num_blocks * WIDTH * sizeof(float));

	/* this preprocessing stage is added to correct the bugs of wrong 
	   memcopy using two-dimensional net->inputweights */
	for (k = 0; k <= in; k++) {	
		for (j = 0; j <= hid; j++) {
			input_weights_one_dim[m] = net->input_weights[k][j];
			input_weights_prev_one_dim[m] = net-> input_prev_weights[k][j];
			m++;
		}
	}

	/*
	 * call our common CUDA initialization utility function.
	 */
    probe_time_start(&ts_total);
    probe_time_start(&ts_init);
	CCA_BENCHMARK_START;
	CCA_INIT;

	res = cuda_driver_api_init(&ctx, &mod, "./backprop.cubin");
	if (res != CUDA_SUCCESS) {
		printf("cuda_driver_api_init failed: res = %u\n", res);
		return ;
	}
    init_time = probe_time_end(&ts_init);
	CCA_INIT_STOP;
	CCA_MEMALLOC;
	/*
	 * allocate device memory space
	 */
    probe_time_start(&ts_memalloc);
	res = cuMemAlloc(&input_cuda, (in + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return ;
	}
	res = cuMemAlloc(&output_hidden_cuda, (hid + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return ;
	}
	res = cuMemAlloc(&input_hidden_cuda, (in + 1) * (hid + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return ;
	}
	res = cuMemAlloc(&hidden_partial_sum, num_blocks * WIDTH * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return ;
	}
	res = cuMemAlloc(&hidden_delta_cuda, (hid + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return ;
	}
	res = cuMemAlloc(&input_prev_weights_cuda, (in + 1) * (hid + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return ;
	}
    mem_alloc_time = probe_time_end(&ts_memalloc);
	CCA_MEMALLOC_STOP;

#endif

#ifdef CPU
	printf("Performing CPU computation\n");
	bpnn_layerforward(net->input_units, net->hidden_units,net->input_weights, in, hid);
#endif

#ifdef GPU

	printf("Performing GPU computation\n");
    //printf("in= %d, hid = %d, numblocks = %d\n", in, hid, num_blocks);
    probe_time_start(&ts_h2d);
	CCA_H_TO_D;


	res = cuMemcpyHtoD(input_cuda, net->input_units, (in + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return ;
	}
	res = cuMemcpyHtoD(input_hidden_cuda, input_weights_one_dim, (in + 1) * (hid + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return ;
	}
    h2d_time = probe_time_end(&ts_h2d);
	CCA_H_TO_D_STOP;
	CCA_EXEC;

    probe_time_start(&ts_kernel);
	res = bpnn_layerforward_launch(mod, input_cuda, output_hidden_cuda,
								   input_hidden_cuda, hidden_partial_sum,
								   in, hid);
	if (res != CUDA_SUCCESS) {
		printf("bpnn_layerforward failed: res = %u\n", res);
		return ;
	}

	cuCtxSynchronize();

#if 0
	cudaError_t error = cudaGetLastError();
	if (error != cudaSuccess) {
		printf("bpnn kernel error: %s\n", cudaGetErrorString(error));
		exit(EXIT_FAILURE);
	}
#endif

    kernel_time = probe_time_end(&ts_kernel);
    probe_time_start(&ts_d2h);

	res = cuMemcpyDtoH(partial_sum, hidden_partial_sum, num_blocks * WIDTH * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyDtoH(layerforward) failed: res = %u\n", res);
		return ;
	}

    d2h_time = probe_time_end(&ts_d2h);

	for (j = 1; j <= hid; j++) {
		sum = 0.0;
		for (k = 0; k < num_blocks; k++) {	
			sum += partial_sum[k * hid + j-1] ;
		}
		sum += net->input_weights[0][j];
		net-> hidden_units[j] = (float) (1.0 / (1.0 + exp(-sum)));
	}
  #endif

	bpnn_layerforward(net->hidden_units, net->output_units, net->hidden_weights, hid, out);
	bpnn_output_error(net->output_delta, net->target, net->output_units, out, &out_err);
	bpnn_hidden_error(net->hidden_delta, hid, net->output_delta, out, net->hidden_weights, net->hidden_units, &hid_err);  
	bpnn_adjust_weights(net->output_delta, out, net->hidden_units, hid, net->hidden_weights, net->hidden_prev_weights);

#ifdef CPU
	bpnn_adjust_weights(net->hidden_delta, hid, net->input_units, in, net->input_weights, net->input_prev_weights);
#endif  

#ifdef GPU
    probe_time_start(&ts_h2d);

	res = cuMemcpyHtoD(hidden_delta_cuda, net->hidden_delta, (hid + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return ;
	}
	res = cuMemcpyHtoD(input_prev_weights_cuda, input_weights_prev_one_dim, (in + 1) * (hid + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return ;
	}
	res = cuMemcpyHtoD(input_hidden_cuda, input_weights_one_dim, (in + 1) * (hid + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return ;
	}

    h2d_time += probe_time_end(&ts_h2d);
    probe_time_start(&ts_kernel);

	res = bpnn_adjust_weights_launch(mod, hidden_delta_cuda, hid, 
									 input_cuda, in, 
									 input_hidden_cuda, 
									 input_prev_weights_cuda);
	if (res != CUDA_SUCCESS) {
		printf("bpnn_adjust_weights failed: res = %u\n", res);
		return ;
	}

    kernel_time += probe_time_end(&ts_kernel);
    probe_time_start(&ts_d2h);
	CCA_EXEC_STOP;
	CCA_D_TO_H;


	res = cuMemcpyDtoH(net->input_units, input_cuda, (in + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyDtoH(adjust_weights) failed: res = %u\n", res);
		return ;
	}

	res = cuMemcpyDtoH(input_weights_one_dim, input_hidden_cuda, (in + 1) * (hid + 1) * sizeof(float));
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyDtoH(adjust_weights) failed: res = %u\n", res);
		return ;
	}

    d2h_time += probe_time_end(&ts_d2h);
	CCA_D_TO_H_STOP;
	CCA_CLOSE;
    probe_time_start(&ts_close);

	cuMemFree(input_cuda);
	cuMemFree(output_hidden_cuda);
	cuMemFree(input_hidden_cuda);
	cuMemFree(hidden_partial_sum);
	cuMemFree(input_prev_weights_cuda);
	cuMemFree(hidden_delta_cuda);

	res = cuda_driver_api_exit(ctx, mod);
	if (res != CUDA_SUCCESS) {
		printf("cuda_driver_api_exit faild: res = %u\n", res);
		return ;
	}

	CCA_CLOSE_STOP;
	CCA_BENCHMARK_STOP;
    close_time = probe_time_end(&ts_close);
	total_time = probe_time_end(&ts_total);

	printf("Init: %f\n", init_time);
	printf("MemAlloc: %f\n", mem_alloc_time);
	printf("HtoD: %f\n", h2d_time);
	printf("Exec: %f\n", kernel_time);
	printf("DtoH: %f\n", d2h_time);
	printf("Close: %f\n", close_time);
	printf("API: %f\n", init_time+mem_alloc_time+h2d_time+kernel_time+d2h_time+close_time);
	printf("Total: %f\n", total_time);

	free(partial_sum);
	free(input_weights_one_dim);
	free(input_weights_prev_one_dim);
#endif   
}
