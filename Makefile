# NOTE: This Makefile is intended for Windows with LLVM only

CC = clang

WFLAGS = -Wall -Wextra -Werror
DFLAGS = -D_CRT_SECURE_NO_WARNINGS -DMAX_MEMORY=8589934592
STD = -std=c99 -pedantic
CFLAGS_RELEASE = $(WFLAGS) -g -O3 $(STD) $(DFLAGS)
CFLAGS_DEBUG = $(WFLAGS) -g $(STD) $(DFLAGS)

LFLAGS = -fuse-ld=lld

TARGET = monotone_chain
TARGET_RELEASE = $(TARGET)_release
TARGET_DEBUG = $(TARGET)_debug
SRCS = src/$(TARGET).c

POINTGEN = pointgen
POINTGEN_RELEASE = $(POINTGEN)_release
POINTGEN_DEBUG = $(POINTGEN)_debug
POINTGEN_SRCS = src/$(POINTGEN).c

all: release debug

release:
	$(CC) $(CFLAGS_RELEASE) $(LFLAGS) $(SRCS) -o $(TARGET_RELEASE).exe

debug:
	$(CC) $(CFLAGS_DEBUG) $(LFLAGS) $(SRCS) -o $(TARGET_DEBUG).exe

pointgen:
	$(CC) $(CFLAGS_RELEASE) $(LFLAGS) $(POINTGEN_SRCS) -o $(POINTGEN_RELEASE).exe
	$(CC) $(CFLAGS_DEBUG) $(LFLAGS) $(POINTGEN_SRCS) -o $(POINTGEN_DEBUG).exe

clean:
	del $(TARGET_RELEASE).* $(TARGET_DEBUG).* $(POINTGEN_DEBUG).* $(POINTGEN_RELEASE).*
