# Compiler and linker
CC = gcc
CFLAGS = -Wall -O3

# Paths
SRC = main.c
OUT = main.exe

# DLL import library (adjust path and name if needed)
LIBS = -L. -lchesslib

# Targets
all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	del /Q $(OUT)
