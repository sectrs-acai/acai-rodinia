# Makefile
TARGET	= user_test
CC=$(CROSS_COMPILE)gcc
NVCC	= nvcc -arch sm_20 -cubin
LIBS	= -lucuda -lgdev
CFLAGS	= -L /usr/local/gdev/lib64 -I /usr/local/gdev/include

all:
	gcc -o $(TARGET) $(CFLAGS) main.c shm.c $(LIBS)

clean:
	rm -f $(TARGET) *.cubin ./*~
