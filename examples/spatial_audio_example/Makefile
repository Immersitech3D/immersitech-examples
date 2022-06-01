# Makefile for Immersitech Example

# *****************  DEFINITIONS

.DEFAULT_GOAL := all

CXX = g++
CC = gcc

# Copy your libimmersitech.so file to the path below
INCLUDE_DIR = ./../../include
IMM_LIB_DIR = ./../../lib

OBJ = obj
BIN = bin

EXEC_CFLAGS := -g -Wall -std=c++17 -I$(INCLUDE_DIR)

.PHONY: clean object_dir bin_dir check

immersitech_example:
	$(CXX) -c -o $(OBJ)/immersitech_example.o immersitech_example.c $(EXEC_CFLAGS)
	$(CXX) -o $(BIN)/$@.exe $(OBJ)/immersitech_example.o $(EXEC_CFLAGS) -pthread -lpthread -L$(IMM_LIB_DIR) -limmersitech -Xlinker -rpath -Xlinker $(IMM_LIB_DIR)

# ***************** Targets

move_license:
	cp ./../../license/*.dat ./Immersitech_Engineering_sound_manager_license_key.dat

object_dir:
	mkdir -p $(OBJ)

bin_dir:
	mkdir -p $(BIN)

check: object_dir bin_dir move_license

clean_dirs:
	rm -r -f $(OBJ)
	rm -r -f $(BIN)

all : check immersitech_example

clean: clean_dirs
