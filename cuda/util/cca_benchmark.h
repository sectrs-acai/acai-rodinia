#ifndef CCA_BENCHMARK_H_
#define CCA_BENCHMARK_H_

#if defined(__x86_64__) || defined(_M_X64)
#define CCA_MARKER(marker)
#else
#define STR(s) #s
#define CCA_MARKER(marker) __asm__ volatile("MOV XZR, " STR(marker))
/*/#define CCA_MARKER(marker) __asm__ volatile("MOV XZR, %0" ::"i"(marker) :) */
#endif


// -----------------------------------------------------------------------
/* for linux: */
#define CCA_INIT CCA_MARKER(0x1010)
#define CCA_INIT_STOP CCA_MARKER(0x1011)

#define CCA_MEMALLOC CCA_MARKER(0x1012)
#define CCA_MEMALLOC_STOP CCA_MARKER(0x1013)

#define CCA_EXEC CCA_MARKER(0x1014)
#define CCA_EXEC_STOP CCA_MARKER(0x1015)

#define CCA_CLOSE CCA_MARKER(0x1016)
#define CCA_CLOSE_STOP CCA_MARKER(0x1017)

#define CCA_H_TO_D CCA_MARKER(0x1018)
#define CCA_H_TO_D_STOP CCA_MARKER(0x1019)

#define CCA_D_TO_H CCA_MARKER(0x101A)
#define CCA_D_TO_H_STOP CCA_MARKER(0x101B)

#ifdef CCA_NO_BENCH
#define IS_CCA_NO_BENCH 1
#define CCA_BENCHMARK_START
#define CCA_BENCHMARK_STOP
#else
#define IS_CCA_NO_BENCH 0
#define CCA_BENCHMARK_START                                             \
  CCA_TRACE_START;                                                             \
  CCA_FLUSH;                                                                             \
  CCA_MARKER(0x1)

#define CCA_BENCHMARK_STOP                                                     \
  CCA_MARKER(0x2);                                                             \
  CCA_FLUSH;                                                                             \
  CCA_TRACE_STOP
#endif



#if defined(__x86_64__) || defined(_M_X64)
#define CCA_FLUSH
#define CCA_BENCHMARK_INIT
#define CCA_TRACE_START
#define CCA_TRACE_STOP

#else
#define CCA_FLUSH __asm__ volatile("ISB");
#define CCA_TRACE_START __asm__ volatile("HLT 0x1337");
#define CCA_TRACE_STOP __asm__ volatile("HLT 0x1337");

// aarch64
#include <signal.h>
#include <ucontext.h>

static void __cca_sighandler(int signo, siginfo_t *si, void *data) {
  ucontext_t *uc = (ucontext_t *)data;
  uc->uc_mcontext.pc += 4;
}

#ifdef ENC_CUDA
#define IS_ENC_CUDA 1
#else
#define IS_ENC_CUDA 0
#endif

#define CCA_BENCHMARK_INIT                                                     \
  {                                                                          \
    printf("enc_cuda: %d\n", IS_ENC_CUDA);                                      \
    printf("cca is no bench: %d\n", IS_CCA_NO_BENCH);                        \
    struct sigaction sa, osa;                                                   \
    sa.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;                        \
    sa.sa_sigaction = __cca_sighandler;                                        \
    sigaction(SIGILL, &sa, &osa);                                              \
  }
#endif

#endif // CCA_BENCHMARK_H_
