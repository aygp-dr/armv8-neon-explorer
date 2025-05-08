# Makefile for NEON explorer tool

CC = gcc
CFLAGS = -Wall -Wextra -O3 -g
ARCH_FLAGS = -march=armv8-a+simd
INCLUDE = -I../include

all: neon_explorer

neon_explorer: neon_explorer.c
	$(CC) $(CFLAGS) $(ARCH_FLAGS) $(INCLUDE) -o neon_explorer neon_explorer.c -lm

clean:
	rm -f neon_explorer

.PHONY: all clean
