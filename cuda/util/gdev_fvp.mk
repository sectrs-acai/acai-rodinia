CROSS_COMPILE ?= aarch64-none-linux-gnu-
CC ?= $(CROSS_COMPILE)gcc
NVCC = nvcc

NVCCFLAGS += -O3 \
	-use_fast_math \
	-arch sm_20 \
	-cubin 

# -L/home/armcca/trusted-peripherals/assets/snapshots/aarch64-lib
CFLAGS += -I$(CUDA_TOP_DIR)/util \
	-I$(STAGING_DIR)/usr/local/gdev/include \
    -L$(STAGING_DIR)/usr/local/gdev/lib64 \
    -lucuda -lgdev \
	-O3 \
	-Wall \

CCFILES += $(CUDA_TOP_DIR)/util/util.c

.PHONY: all clean

all: gcc

gcc:
	$(CC) -o $(EXECUTABLE) $(CCFILES) $(CFLAGS)
nvcc:
	$(NVCC) -o $(EXECUTABLE).cubin $(NVCCFLAGS) $(CUFILES)
clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).cubin *~
