# GBS Shell Makefile

CC = g++
CFLAGS  = -Wall -g
OBJ = gbsh.o

all: gbsh

gbsh: $(OBJ)
	$(CC) $(CFLAGS) -o gbsh $(OBJ)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<
