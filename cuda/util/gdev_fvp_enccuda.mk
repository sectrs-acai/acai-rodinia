CROSS_COMPILE ?= aarch64-none-linux-gnu-
CC ?= $(CROSS_COMPILE)gcc
NVCC = nvcc

NVCCFLAGS += -O3 \
	-use_fast_math \
	-arch sm_20 \
	-cubin 

ENC_CUDA_DIR=$(CUDA_TOP_DIR)/util/


# XXX: enc cuda is copied into rootfs at /usr/{include,lib,share}
CFLAGS += -I$(CUDA_TOP_DIR)/util \
	-I$(STAGING_DIR)/usr/local/gdev/include \
	-I$(STAGING_DIR)/usr/include \
	-L$(STAGING_DIR)/usr/lib \
	-L$(STAGING_DIR)/usr/local/gdev/lib64 \
	-lenccuda -lucuda -lgdev \
	-O3 \
	-Wall \
	-DENC_CUDA

CCFILES += $(CUDA_TOP_DIR)/util/util.c

.PHONY: all clean

all: gcc

gcc:
	$(CC) -o $(EXECUTABLE) $(CCFILES) $(CFLAGS)
nvcc:
	$(NVCC) -o $(EXECUTABLE).cubin $(NVCCFLAGS) $(CUFILES)
clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).cubin *~
