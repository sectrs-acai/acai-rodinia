include ../util/make.mk

#C compiler
CC = g++


FLAGS = -O3
SRC = streamcluster.cpp ../util/timing.c
EXE = streamcluster.out

release:$(SRC)
	$(CC) $(SRC) -o $(EXE) \
		-L$(OPENCL_LIB)	$(FLAGS) -l$(LIB_NAME) \
		-I../util -DTIMING \
		-Wl,-rpath,$(OPENCL_LIB) \
		-ldl \

errmsg:$(SRC)
	$(CC) -lOpenCL $(SRC) -o $(EXE) -I$(OPENCL_INC) -L$(OPENCL_LIB) -D ERRMSG $(FLAGS)
	
ptx:$(SRC)
	$(CC) -lOpenCL $(SRC) -o $(EXE) -I$(OPENCL_INC) -L$(OPENCL_LIB) -D PTX_MSG $(FLAGS)
	
res:$(SRC)
	$(CC) -lOpenCL $(SRC) -o $(EXE) -I$(OPENCL_INC) -L$(OPENCL_LIB) -D RES_MSG $(FLAGS)
	
clean: $(SRC)
	rm -f $(EXE) $(EXE).linkinfo result*
