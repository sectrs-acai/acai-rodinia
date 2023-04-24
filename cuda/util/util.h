#ifndef __UTIL_H__
#define __UTIL_H__

#include <cuda.h>
#include <sys/time.h>

#include "cca_benchmark.h"

#ifdef __cplusplus
extern "C" {
#include <enc_cuda/enc_cuda.h>
}
#else
#include <enc_cuda/enc_cuda.h>
#endif


CUresult cuda_driver_api_init(CUcontext *pctx, CUmodule *pmod, const char *f);
CUresult cuda_driver_api_exit(CUcontext ctx, CUmodule mod);
void time_measure_start(struct timeval *tv);
void time_measure_end(struct timeval *tv);

/* tvsub: ret = x - y. */
static inline void tvsub(struct timeval *x, 
						 struct timeval *y, 
						 struct timeval *ret)
{
	ret->tv_sec = x->tv_sec - y->tv_sec;
	ret->tv_usec = x->tv_usec - y->tv_usec;
	if (ret->tv_usec < 0) {
		ret->tv_sec--;
		ret->tv_usec += 1000000;
	}
}

struct timestamp {
    struct timeval start;
    struct timeval end;
};

void probe_time_start(struct timestamp *ts);
float probe_time_end(struct timestamp *ts);

#endif
