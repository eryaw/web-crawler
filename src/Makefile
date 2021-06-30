##Adapted from Lab2 COMP30023 Computer System 2020
CC = gcc

CFLAGS = -Wall -Wextra -std=gnu99 -I. #-g 

OBJ = main.o socketHandler.o httpHandler.o htmlHandler.o urlHandler.o \
    	responseInfo.o dlist.o fetchHandler.o urlInfo.o utilities.o
EXE = crawler

## Create .o files from .c files. Searches for .c files with same .o names given in OBJ
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

## Create executable linked file from object files. 
$(EXE): $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

## Run "$ make clean" to remove the object and executable files
clean:
	rm -f $(OBJ) $(EXE)

