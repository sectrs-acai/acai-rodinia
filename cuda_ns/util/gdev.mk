ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif
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
    -L/home/armcca/trusted-peripherals/src/encrypted-cuda/install/lib \
	-I/home/armcca/trusted-peripherals/src/encrypted-cuda/install/include \
	-O3 \
	-Wall

ifndef ENC_CUDA
CFLAGS += -lucuda -lgdev
$(warning "ENC_CUDA not set, compiling without enc cuda")
else
CFLAGS += -lenccuda  -lucuda -lgdev -DENC_CUDA
$(warning "ENC_CUDA IS set")
endif

CCFILES += $(CUDA_TOP_DIR)/util/util.c

.PHONY: all clean

all: gcc

nvcc:
	$(NVCC) -o $(EXECUTABLE).cubin $(NVCCFLAGS) $(CUFILES)

gcc:
	$(CC) -o $(EXECUTABLE) $(CCFILES) $(CFLAGS)

clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).cubin *~
