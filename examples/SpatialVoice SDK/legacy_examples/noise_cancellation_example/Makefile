# Makefile for Immersitech Usage Example

# *****************  DEFINITIONS

.DEFAULT_GOAL := all

CXX = g++
CC = gcc

MAIN_ROOT = .
INCLUDE_DIR = ./../../include
IMM_LIB_DIR = ./../../lib

OBJ = obj
BIN = bin

HEADER = $(INCLUDE_DIR)
HEADER_INCS := $(wildcard $(HEADER)/*.h)

EXEC_CFLAGS := -g -Wall -std=c++17 -I$(HEADER)

.PHONY: clean object_dir bin_dir check

noise_cancellation_example:
	$(CXX) -c -o $(OBJ)/noise_cancellation_example.o noise_cancellation_example.c $(EXEC_CFLAGS)
	$(CXX) -o $(BIN)/$@.exe $(OBJ)/noise_cancellation_example.o $(EXEC_CFLAGS) -L$(IMM_LIB_DIR) -limmersitech -Xlinker -rpath -Xlinker $(IMM_LIB_DIR)

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

all : check noise_cancellation_example

clean: clean_dirs
