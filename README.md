# Monotone Chain Convex Hull Solver

## Overview
C implementation of the monotone chain convex hull solver. The solver takes a raw binary array of 2D points, consisting of signed 32-bit integers as input. Sample input files consisting of random points can be generated using pointgen.c.

## Building
The solver and the point generation utility can be built by compiling `monotone_chain.c` and `pointgen.c`, respectively. The programs can be built for Windows or any POSIX-compliant platform supporting C99. An example Makefile is provided, targeting Windows with the LLVM toolchain.

The required memory for the program is 8 gigabytes by default, but can be overridden by defining MAX_MEMORY in the compilation flags. 

## Current Limitations
- The program does not currently check for degenerate edges. Arrays of densely clustered points will get errors when being evaluated.
- Only 32-bit signed integers are supported for point components. Floating point and other size integer types are not currently supported.
