#
# The following must be provided:
#

# A path to CUDA top directory
CUDA_TOP_DIR	:= ../
# An executable name
EXECUTABLE	:= srad2
# Cuda source files (compiled with nvcc)
CUFILES		:= srad_cuda_kernels.cu
# C/C++ source files (compiled with gcc / c++)
CCFILES		:= srad_cuda.c

# init C flags (more options will be added)
CFLAGS		:= -lm

include $(CUDA_TOP_DIR)/util/gdev.mk
