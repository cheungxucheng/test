###############################################################
# Class:: CSC-615-01 Fall 2026
# Name:: Ryan Zhang
# Student ID:: 923578900
# Github-Name:: cheungxucheng
# Project:: Assignment 4 - Follow the Line but Stop
#
# File:: Makefile
#
# Description:: 
# This file facilitates the compilation of the main file
##############################################################/
LASTNAME=Zhang
FIRSTNAME=Ryan
ASSIGNNO=4

FILENAME=main
CC=gcc
CFLAGS= -g -I.
LIBS =-lgpiod -lm
DEPS = 
ARCH = $(shell uname -m)

ADDOBJ = TCS34725.o

OBJ = $(FILENAME).o $(ADDOBJ)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(FILENAME): $(OBJ)
	$(CC) -o main $^ $(CFLAGS)  $(LIBS)

run: $(FILENAME)
	sudo ./main

vrun: $(FILENAME)
	sudo valgrind ./main

clean:
	rm *.o main

