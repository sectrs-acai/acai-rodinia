//    2006.03   Rob Janiczek
//        --creation of prototype version
//    2006.03   Drew Gilliam
//        --rewriting of prototype version into current version
//        --got rid of multiple function calls, all code in a  
//         single function (for speed)
//        --code cleanup & commenting
//        --code optimization efforts   
//    2006.04   Drew Gilliam
//        --added diffusion coefficent saturation on [0,1]
//		2009.12 Lukasz G. Szafaryn
//		-- reading from image, command line inputs
//		2010.01 Lukasz G. Szafaryn
//		--comments
//    2011.12 Shinpei Kato
//        --modified to use Driver API

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cuda.h>
#include "util.h"
#include "srad.h"

#include "graphics.c"
#include "resize.c"
#include "timer.c"

/* (in library path specified to compiler)	needed by for device functions */
#include "device.c"	

CUresult extract_launch
(CUmodule mod, int gdx, int gdy, int bdx, int bdy, long Ne, CUdeviceptr d_I)
{
	CUfunction f;
	CUresult res;
	void* param[] = {&Ne, &d_I, NULL};

	res = cuModuleGetFunction(&f, mod, "_Z7extractlPf");
	if (res != CUDA_SUCCESS) {
		printf("cuModuleGetFunction(extract) failed: res = %u\n", res);
		return res;
	}
	
	res = cuLaunchKernel(f, gdx, gdy, 1, bdx, bdy, 1, 0, 0, (void**)param, 0);
	if (res != CUDA_SUCCESS) {
		printf("cuLaunchKernel(extract) failed: res = %u\n", res);
		return res;
	}

	return CUDA_SUCCESS;
}

CUresult prepare_launch
(CUmodule mod, int gdx, int gdy, int bdx, int bdy, long Ne, CUdeviceptr d_I,
 CUdeviceptr d_sums, CUdeviceptr d_sums2)
{
	CUfunction f;
	CUresult res;
	void* param[] = {&Ne, &d_I, &d_sums, &d_sums2, NULL};

	/* get functions. */
	res = cuModuleGetFunction(&f, mod, "_Z7preparelPfS_S_");
	if (res != CUDA_SUCCESS) {
		printf("cuModuleGetFunction(prepare) failed: res = %u\n", res);
		return res;
	}
	
	res = cuLaunchKernel(f, gdx, gdy, 1, bdx, bdy, 1, 0, 0, (void**)param, 0);
	if (res != CUDA_SUCCESS) {
		printf("cuLaunchKernel(prepare) failed: res = %u\n", res);
		return res;
	}

	return CUDA_SUCCESS;
}

CUresult reduce_launch
(CUmodule mod, int gdx, int gdy, int bdx, int bdy, long Ne, int no, int mul, 
 CUdeviceptr d_sums, CUdeviceptr d_sums2)
{
	CUfunction f;
	CUresult res;
	void* param[] = {&Ne, &no, &mul, &d_sums, &d_sums2, NULL};

	/* get functions. */
	res = cuModuleGetFunction(&f, mod, "_Z6reduceliiPfS_");
	if (res != CUDA_SUCCESS) {
		printf("cuModuleGetFunction(reduce) failed: res = %u\n", res);
		return res;
	}
	
	res = cuLaunchKernel(f, gdx, gdy, 1, bdx, bdy, 1, 0, 0, (void**)param, 0);
	if (res != CUDA_SUCCESS) {
		printf("cuLaunchKernel(reduce) failed: res = %u\n", res);
		return res;
	}

	return CUDA_SUCCESS;
}

CUresult srad_launch
(CUmodule mod, int gdx, int gdy, int bdx, int bdy, fp lambda, int Nr, int Nc, 
 long Ne, CUdeviceptr d_iN, CUdeviceptr d_iS, CUdeviceptr d_jE, 
 CUdeviceptr d_jW, CUdeviceptr d_dN, CUdeviceptr d_dS, CUdeviceptr d_dE, 
 CUdeviceptr d_dW, fp q0sqr, CUdeviceptr d_c, CUdeviceptr d_I)
{
	CUfunction f;
	CUresult res;
	void* param[] = {&lambda, &Nr, &Nc, &Ne, &d_iN, &d_iS, &d_jE, &d_jW, &d_dN,
					 &d_dS, &d_dE, &d_dW, &q0sqr, &d_c, &d_I, NULL};

	/* get functions. */
	res = cuModuleGetFunction(&f, mod, "_Z4sradfiilPiS_S_S_PfS0_S0_S0_fS0_S0_");
	if (res != CUDA_SUCCESS) {
		printf("cuModuleGetFunction(srad) failed: res = %u\n", res);
		return res;
	}
	
	res = cuLaunchKernel(f, gdx, gdy, 1, bdx, bdy, 1, 0, 0, (void**)param, 0);
	if (res != CUDA_SUCCESS) {
		printf("cuLaunchKernel(srad) failed: res = %u\n", res);
		return res;
	}

	return CUDA_SUCCESS;
}

CUresult srad2_launch
(CUmodule mod, int gdx, int gdy, int bdx, int bdy, fp lambda, int Nr, int Nc, 
 long Ne, CUdeviceptr d_iN, CUdeviceptr d_iS, CUdeviceptr d_jE,  
 CUdeviceptr d_jW, CUdeviceptr d_dN, CUdeviceptr d_dS, CUdeviceptr d_dE, 
 CUdeviceptr d_dW, CUdeviceptr d_c, CUdeviceptr d_I)
{
	CUfunction f;
	CUresult res;
	void* param[] = {&lambda, &Nr, &Nc, &Ne, &d_iN, &d_iS, &d_jE, &d_jW, &d_dN,
					 &d_dS, &d_dE, &d_dW, &d_c, &d_I, NULL};

	/* get functions. */
	res = cuModuleGetFunction(&f, mod, "_Z5srad2fiilPiS_S_S_PfS0_S0_S0_S0_S0_");
	if (res != CUDA_SUCCESS) {
		printf("cuModuleGetFunction(srad2) failed: res = %u\n", res);
		return res;
	}
	
	res = cuLaunchKernel(f, gdx, gdy, 1, bdx, bdy, 1, 0, 0, (void**)param, 0);
	if (res != CUDA_SUCCESS) {
		printf("cuLaunchKernel(srad2) failed: res = %u\n", res);
		return res;
	}

	return CUDA_SUCCESS;
}

CUresult compress_launch
(CUmodule mod, int gdx, int gdy, int bdx, int bdy, long Ne, CUdeviceptr d_I)
{
	CUfunction f;
	CUresult res;
	void* param[] = {&Ne, &d_I, NULL};

	res = cuModuleGetFunction(&f, mod, "_Z8compresslPf");
	if (res != CUDA_SUCCESS) {
		printf("cuModuleGetFunction(compress) failed: res = %u\n", res);
		return res;
	}
	
	res = cuLaunchKernel(f, gdx, gdy, 1, bdx, bdy, 1, 0, 0, (void**)param, 0);
	if (res != CUDA_SUCCESS) {
		printf("cuLaunchKernel(compress) failed: res = %u\n", res);
		return res;
	}

	return CUDA_SUCCESS;
}

int main(int argc, char *argv [])
{
	CCA_BENCHMARK_INIT;
	/* time */
    struct timestamp ts_init, ts_total, ts_memalloc, ts_h2d, ts_d2h, ts_kernel, ts_close;
    float init_time = 0, mem_alloc_time = 0, h2d_time = 0, kernel_time = 0,
          d2h_time = 0, close_time = 0, total_time = 0;

	/* inputs image, input paramenters */
	fp* image_ori; /* originalinput image */
	int image_ori_rows;
	int image_ori_cols;
	long image_ori_elem;

	/* inputs image, input paramenters */
	fp* image; /* input image */
	int Nr, Nc;	/* IMAGE nbr of rows/cols/elements */
	long Ne;

	/* algorithm parameters */
	int niter; /* nbr of iterations */
	fp lambda; /* update step size */

	/* size of IMAGE */
	int r1, r2, c1, c2; /* row/col coordinates of uniform ROI */
	long NeROI;	/* ROI nbr of elements */

	/* surrounding pixel indicies */
	int *iN, *iS, *jE, *jW;	
	
	/* counters */
	int iter; /* primary loop */
	long i, j; /* image row/col */

	/* memory sizes */
	int mem_size_i;
	int mem_size_j;
	int mem_size_single;

	/*******************************************************
	 * 	GPU VARIABLES
	 ******************************************************/
	/* CUDA kernel execution parameters */
	int bdx, bdy;
	int x;
	int gdx, gdy;
	int gdx2, gdy2;

	/*  memory sizes */
	int mem_size; /* matrix memory size */

	/* HOST */
	int no;
	int mul;
	fp total;
	fp total2;
	fp meanROI;
	fp meanROI2;
	fp varROI;
	fp q0sqr;

	/* DEVICE */
	CUdeviceptr d_sums;	/* partial sum */
	CUdeviceptr d_sums2;
	CUdeviceptr d_iN;
	CUdeviceptr d_iS;
	CUdeviceptr d_jE;
	CUdeviceptr d_jW;
	CUdeviceptr d_dN; 
	CUdeviceptr d_dS; 
	CUdeviceptr d_dW; 
	CUdeviceptr d_dE;
	CUdeviceptr d_I; /* input IMAGE on DEVICE */
	CUdeviceptr d_c;

	CUcontext ctx;
	CUmodule mod;
	CUresult res;

	/*******************************************************
	 * 	GET INPUT PARAMETERS
	 ******************************************************/
	if(argc != 5) {
		printf("ERROR: wrong number of arguments\n");
		return 0;
	}
	else {
		niter = atoi(argv[1]);
		lambda = atof(argv[2]);
		Nr = atoi(argv[3]);	// it is 502 in the original image
		Nc = atoi(argv[4]);	// it is 458 in the original image
	}

	/*******************************************************
	 * READ IMAGE (SIZE OF IMAGE HAS TO BE KNOWN)
	 ******************************************************/
	/* read image */
	image_ori_rows = 502;
	image_ori_cols = 458;
	image_ori_elem = image_ori_rows * image_ori_cols;

	image_ori = (fp*) malloc(sizeof(fp) * image_ori_elem);

	read_graphics("../../../data/srad/image.pgm",
				  image_ori, image_ori_rows, image_ori_cols, 1);

	/*******************************************************
	 * RESIZE IMAGE (ASSUMING COLUMN MAJOR STORAGE OF image_orig)
	 ******************************************************/
	Ne = Nr * Nc;

	image = (fp*) malloc(sizeof(fp) * Ne);

	resize(image_ori, image_ori_rows, image_ori_cols, image, Nr, Nc, 1);

	/*******************************************************
	 * SETUP
	 ******************************************************/
	r1 = 0;	/* top row index of ROI */
	r2 = Nr - 1; /* bottom row index of ROI */
	c1 = 0;	/* left column index of ROI */
	c2 = Nc - 1; /* right column index of ROI */

	/* ROI image size */
	NeROI = (r2 - r1 + 1) * (c2 - c1 + 1); /* # of elements in ROI, ROI size */

	/* allocate variables for surrounding pixels */
	mem_size_i = sizeof(int) * Nr;
	iN = (int *) malloc(mem_size_i); /* north surrounding element */
	iS = (int *) malloc(mem_size_i); /* south surrounding element */
	mem_size_j = sizeof(int) * Nc;
	jW = (int *) malloc(mem_size_j); /* west surrounding element */
	jE = (int *) malloc(mem_size_j); /* east surrounding element */

	/* N/S/W/E indices of surrounding pixels (every element of IMAGE) */
	for (i = 0; i < Nr; i++) {
		iN[i] = i - 1; /* holds index of IMAGE row above */
		iS[i] = i + 1; /* holds index of IMAGE row below */
	}
	for (j = 0; j < Nc; j++) {
		jW[j] = j - 1; /* holds index of IMAGE column on the left */
		jE[j] = j + 1; /* holds index of IMAGE column on the right */
	}

	/* N/S/W/E boundary conditions, 
	   fix surrounding indices outside boundary of image */
	iN[0] = 0; /* changes IMAGE top row index from -1 to 0 */
	iS[Nr - 1] = Nr - 1; /* changes IMAGE bottom row index from Nr to Nr-1 */
	jW[0] = 0; /* changes IMAGE leftmost column index from -1 to 0 */
	jE[Nc - 1] = Nc - 1; /* changes IMAGE rightmost col idx from Nc to Nc-1 */

	/*******************************************************
	 * GPU SETUP
	 ******************************************************/

	/* call our common CUDA initialization utility function. */
	CCA_BENCHMARK_START;
	CCA_INIT;
	probe_time_start(&ts_total);
    probe_time_start(&ts_init);

	res = cuda_driver_api_init(&ctx, &mod, "./srad.cubin");
	if (res != CUDA_SUCCESS) {
		printf("cuda_driver_api_init failed: res = %u\n", res);
		return -1;
	}

    init_time = probe_time_end(&ts_init);
	CCA_INIT_STOP;
	CCA_MEMALLOC;
    probe_time_start(&ts_memalloc);

	/* allocate memory for entire IMAGE on DEVICE */
	mem_size = sizeof(fp) * Ne;	/* size of input IMAGE */
	res = cuMemAlloc(&d_I, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}

	/* allocate memory for coordinates on DEVICE */
	res = cuMemAlloc(&d_iN, mem_size_i);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}
	res = cuMemAlloc(&d_iS, mem_size_i);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}
	res = cuMemAlloc(&d_jE, mem_size_j);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}
	res = cuMemAlloc(&d_jW, mem_size_j);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}

	/* allocate memory for partial sums on DEVICE */
	res = cuMemAlloc(&d_sums, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}
	res = cuMemAlloc(&d_sums2, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}

	/* allocate memory for derivatives */
	res = cuMemAlloc(&d_dN, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}
	res = cuMemAlloc(&d_dS, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}
	res = cuMemAlloc(&d_dW, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}
	res = cuMemAlloc(&d_dE, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}

	/* allocate memory for coefficient on DEVICE */
	res = cuMemAlloc(&d_c, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemAlloc failed: res = %u\n", res);
		return -1;
	}

    mem_alloc_time = probe_time_end(&ts_memalloc);
	CCA_MEMALLOC_STOP;
	CCA_H_TO_D;

	/*******************************************************
	 * COPY DATA TO DEVICE 
	 ******************************************************/
    probe_time_start(&ts_h2d);

	res = cuMemcpyHtoD(d_iN, iN, mem_size_i);
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return -1;
	}
	res = cuMemcpyHtoD(d_iS, iS, mem_size_i);
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return -1;
	}
	res = cuMemcpyHtoD(d_jE, jE, mem_size_j);
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return -1;
	}
	res = cuMemcpyHtoD(d_jW, jW, mem_size_j);
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return -1;
	}

	/* checkCUDAError("setup"); */

	/*******************************************************
	 * KERNEL EXECUTION PARAMETERS
	 ******************************************************/

	/* all kernels operating on entire matrix */
	bdx = NUMBER_THREADS; /* define # of threads in the block */
	bdy = 1;
	x = Ne / bdx;
	/* compensate for division remainder above by adding one grid */
	if (Ne % bdx != 0) {
		x = x + 1;
	}
	gdx = x; /* define # of blocks in the grid */
	gdy = 1;

	/*******************************************************
	 * COPY INPUT TO GPU
	 ******************************************************/

	res = cuMemcpyHtoD(d_I, image, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyHtoD failed: res = %u\n", res);
		return -1;
	}

    h2d_time = probe_time_end(&ts_h2d);
	CCA_H_TO_D_STOP;
	CCA_EXEC;
	/*******************************************************
	 * SCALE IMAGE DOWN FROM 0-255 TO 0-1 AND EXTRACT
	 ******************************************************/
    probe_time_start(&ts_kernel);

	res = extract_launch(mod, gdx, gdy, bdx, bdy, Ne, d_I);
	if (res != CUDA_SUCCESS) {
		printf("extract_launch failed: res = %u\n", res);
		return -1;
	}

    cuCtxSynchronize();
    kernel_time = probe_time_end(&ts_kernel);

	/* checkCUDAError("extract"); */

	/*******************************************************
	 * COMPUTATION
	 ******************************************************/
	/* execute main loop:
	   do for # of iterations input parameter */
	for (iter = 0; iter < niter; iter++) {
		/* printf("%d ", iter); */
		/* fflush(NULL); */

        probe_time_start(&ts_kernel);
		/* execute square kernel */
		res = prepare_launch(mod, gdx, gdy, bdx, bdy, Ne, d_I, d_sums, d_sums2);
		if (res != CUDA_SUCCESS) {
			printf("prepare_launch failed: res = %u\n", res);
			return -1;
		}

		/* checkCUDAError("prepare"); */

		/* performs subsequent reductions of sums */
		gdx2 = gdx; /* original number of blocks */
		gdy2 = gdy;
		no = Ne; /* original number of sum elements */
		mul = 1; /* original multiplier */

		while (gdx2 != 0) {
			/* checkCUDAError("before reduce"); */
			/* run kernel */
			res = reduce_launch(mod, gdx2, gdy2, bdx, bdy, Ne, no, mul, 
								d_sums, d_sums2);
			if (res != CUDA_SUCCESS) {
				printf("reduce_launch failed: res = %u\n", res);
				return -1;
			}

			/* checkCUDAError("reduce"); */

			/* update execution parameters */
			no = gdx2; /* get current number of elements */
			if (gdx2 == 1) {
				gdx2 = 0;
			}
			else {
				mul = mul * NUMBER_THREADS;	/* update the increment */
				x = gdx2 / bdx;	/* # of blocks */
				/* compensate for division remainder above by adding one grid */
				if (gdx2 % bdx != 0) {
					x = x + 1;
				}
				gdx2 = x;
				gdy2 = 1;
			}
			/* checkCUDAError("after reduce"); */

		}

		/* checkCUDAError("before copy sum"); */
        cuCtxSynchronize();

        kernel_time += probe_time_end(&ts_kernel);

		/* copy total sums to HOST */
		mem_size_single = sizeof(fp) * 1;
        probe_time_start(&ts_d2h);
		res = cuMemcpyDtoH(&total, d_sums, mem_size_single);
		if (res != CUDA_SUCCESS) {
			printf("cuMemcpyDtoH failed: res = %u\n", res);
			return -1;
		}
		res = cuMemcpyDtoH(&total2, d_sums2, mem_size_single);
		if (res != CUDA_SUCCESS) {
			printf("cuMemcpyDtoH failed: res = %u\n", res);
			return -1;
		}
        d2h_time += probe_time_end(&ts_d2h);

		/* checkCUDAError("copy sum"); */
		/* calculate statistics */
		meanROI	= total / (fp)(NeROI); /* mean (avg.) value of element in ROI */
		meanROI2 = meanROI * meanROI;
		varROI = (total2 / (fp)(NeROI)) - meanROI2; /* variance of ROI */
		q0sqr = varROI / meanROI2; /* standard deviation of ROI */

        probe_time_start(&ts_kernel);
		/* execute srad kernel */
		res = srad_launch(mod, gdx, gdy, bdx, bdy,
						  lambda, // SRAD coefficient 
						  Nr, // # of rows in input image
						  Nc, // # of columns in input image
						  Ne, // # of elements in input image
						  d_iN,	// indices of North surrounding pixels
						  d_iS, // indices of South surrounding pixels
						  d_jE, // indices of East surrounding pixels
						  d_jW,	// indices of West surrounding pixels
						  d_dN,	// North derivative
						  d_dS,	// South derivative
						  d_dE,	// East derivative
						  d_dW,	// West derivative
						  q0sqr, // standard deviation of ROI 
						  d_c, // diffusion coefficient
						  d_I // output image
			);
		if (res != CUDA_SUCCESS) {
			printf("srad_launch failed: res = %u\n", res);
			return -1;
		}
		/* checkCUDAError("srad"); */
		
		/* execute srad2 kernel */
		res = srad2_launch(mod, gdx, gdy, bdx, bdy,
						   lambda,	// SRAD coefficient 
						   Nr, // # of rows in input image
						   Nc, // # of columns in input image
						   Ne, // # of elements in input image
						   d_iN, // indices of North surrounding pixels
						   d_iS, // indices of South surrounding pixels
						   d_jE, // indices of East surrounding pixels
						   d_jW, // indices of West surrounding pixels
						   d_dN, // North derivative
						   d_dS, // South derivative
						   d_dE, // East derivative
						   d_dW, // West derivative
						   d_c, // diffusion coefficient
						   d_I // output image
			);
		if (res != CUDA_SUCCESS) {
			printf("srad2_launch failed: res = %u\n", res);
			return -1;
		}
		/* checkCUDAError("srad2"); */

        cuCtxSynchronize();
        kernel_time += probe_time_end(&ts_kernel);
	}

	/*******************************************************
	 * SCALE IMAGE UP FROM 0-1 TO 0-255 AND COMPRESS
	 ******************************************************/

    probe_time_start(&ts_kernel);

	res = compress_launch(mod, gdx, gdy, bdx, bdy, Ne, d_I);
	if (res != CUDA_SUCCESS) {
		printf("compress_launch failed: res = %u\n", res);
		return -1;
	}
	/* checkCUDAError("compress"); */
    cuCtxSynchronize();
    kernel_time += probe_time_end(&ts_kernel);

	CCA_EXEC_STOP;
	CCA_D_TO_H;
	/*******************************************************
	 * COPY RESULTS BACK TO CPU
	 ******************************************************/
    probe_time_start(&ts_d2h);


	res = cuMemcpyDtoH(image, d_I, mem_size);
	if (res != CUDA_SUCCESS) {
		printf("cuMemcpyDtoH failed: res = %u\n", res);
		return -1;
	}
	/* checkCUDAError("copy back"); */
	//cuCtxSynchronize();
    d2h_time += probe_time_end(&ts_d2h);

	CCA_D_TO_H_STOP;
	CCA_CLOSE;
	/*******************************************************
	 * CLEAN UP GPU
	 ******************************************************/

    probe_time_start(&ts_close);

	cuMemFree(d_I);
	cuMemFree(d_c);
	cuMemFree(d_iN);
	cuMemFree(d_iS);
	cuMemFree(d_jE);
	cuMemFree(d_jW);
	cuMemFree(d_dN);
	cuMemFree(d_dS);
	cuMemFree(d_dE);
	cuMemFree(d_dW);
	cuMemFree(d_sums);
	cuMemFree(d_sums2);

	res = cuda_driver_api_exit(ctx, mod);
	if (res != CUDA_SUCCESS) {
		printf("cuda_driver_api_exit faild: res = %u\n", res);
		return -1;
	}

    close_time = probe_time_end(&ts_close);
	total_time = probe_time_end(&ts_total);

	CCA_CLOSE_STOP;
	CCA_BENCHMARK_STOP;

    printf("Init: %f\n", init_time);
	printf("MemAlloc: %f\n", mem_alloc_time);
	printf("HtoD: %f\n", h2d_time);
	printf("Exec: %f\n", kernel_time);
	printf("DtoH: %f\n", d2h_time);
	printf("Close: %f\n", close_time);
	printf("API: %f\n", init_time+mem_alloc_time+h2d_time+kernel_time+d2h_time+close_time);
	printf("Total: %f\n", total_time);

	/*******************************************************
	 * WRITE IMAGE AFTER PROCESSING
	 ******************************************************/

	write_graphics("image_out.pgm", image, Nr, Nc, 1, 255);

	/*******************************************************
	 * DEALLOCATE
	 ******************************************************/
	free(image_ori);
	free(image);
	free(iN);
	free(iS);
	free(jW);
	free(jE);

	return 0;
}
