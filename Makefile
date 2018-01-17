CC = g++
OPT = -O3
OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

SIM_SRC = main.cpp btb.cpp bimodal.cpp 

SIM_OBJ = main.o btb.o bimodal.o

all: sim
	    @echo "my work is done here..."

sim: $(SIM_OBJ)
	$(CC) -o sim_bp $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH SIM-----------"
 
.cc.o:
	$(CC) $(CFLAGS)  -c $*.cc

clean:
	rm -f *.o sim_bp

clobber:
	rm -f *.o


