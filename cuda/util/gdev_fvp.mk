CROSS_COMPILE ?= aarch64-none-linux-gnu-
CC ?= $(CROSS_COMPILE)gcc
NVCC = nvcc

NVCCFLAGS += -O3 \
	-use_fast_math \
	-arch sm_20 \
	-cubin

# staging_dir is build directory of buildroot
# run this sourced in buildroot env!
CFLAGS += -I$(CUDA_TOP_DIR)/util \
	-I$(STAGING_DIR)/usr/local/gdev/include \
	-L$(STAGING_DIR)/usr/local/gdev/lib64 \
	-L$(STAGING_DIR)/usr/lib  \
    -I$(STAGING_DIR)/usr/include \
	-O3 \
	-Wall

define n

==========================

endef

ifndef ENC_CUDA
CFLAGS += -lucuda -lgdev

$(warning $n ENC_CUDA not set, compiling without enc cuda $n)
else
CFLAGS += -lenccuda  -lucuda -lgdev -DENC_CUDA
$(warning $n ENC_CUDA is set $n)
endif

ifdef CCA_NO_BENCH
CFLAGS += -DCCA_NO_BENCH
$(warning $n DCCA_NO_BENCH is set $n)
endif

CCFILES += $(CUDA_TOP_DIR)/util/util.c

.PHONY: all clean

all: gcc

gcc:
	$(CC) -o $(EXECUTABLE) $(CCFILES) $(CFLAGS)
nvcc:
	$(NVCC) -o $(EXECUTABLE).cubin $(NVCCFLAGS) $(CUFILES)
clean:
	rm -f $(EXECUTABLE)  *~

clean-cubin:
	rm -f *~$(EXECUTABLE).cubin