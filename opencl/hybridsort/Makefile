include ../util/make.mk

CC = gcc
CC_FLAGS = -std=c99

ifdef VERIFY
override VERIFY = -DVERIFY
endif

ifdef OUTPUT
override OUTPUT = -DOUTPUT
endif

ifdef TIMER
override TIMER = -DTIMER
endif

hybridsort: hybridsort.c bucketsort.h mergesort.h bucketsort.c mergesort.c
	$(CC) $(CC_FLAGS) -o hybridsort.out $(VERIFY) $(OUTPUT) $(TIMER) \
		-I$(OPENCL_INC) -I../util \
		$(HISTO_WG_SIZE_0) $(BUCKET_WG_SIZE_0) $(BUCKET_WG_SIZE_1) $(MERGE_WG_SIZE_0) $(MERGE_WG_SIZE_1) \
		hybridsort.c bucketsort.c mergesort.c ../util/timing.c \
		-L$(OPENCL_LIB) -l$(LIB_NAME) -lm \
		-DTIMING \
		-Wl,-rpath,$(OPENCL_LIB) \
		-ldl \

clean:
	rm -f  hybridsort.out
