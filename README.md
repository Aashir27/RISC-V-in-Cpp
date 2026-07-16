# RISC-V-in-Cpp

RISC-V-in-Cpp is a small educational RISC-V simulator written in C++. It models a simple processor core, loads a program from a hex instruction file, executes the instructions sequentially, and prints the register file and memory state so you can inspect what the processor is doing at each stage.

The project was built to make the control flow of a basic CPU easy to follow. It is not a full RISC-V implementation, but it supports enough of the ISA to run small arithmetic programs, memory tests, conditional branches, and simple loops such as bubble sort.

## Features

- 64-bit general-purpose registers
- Byte-addressed memory with 64-bit load/store operations
- Program loading from `data/instructions.txt`
- Register file and data memory snapshots for debugging
- Support for a useful subset of RISC-V RV64 instructions
- A simple C++ implementation that is easy to read and extend

## Supported Instructions

The processor currently supports these instructions:

- `addi`
- `add`
- `sub`
- `and`
- `or`
- `ld`
- `sd`
- `beq`
- `bne`
- `blt`
- `bge`
- `bltu`
- `bgeu`
- `jal`
- `jalr`

## Current ISA Notes

- `jal` and `jalr` write the return address to the destination register.
- Branches are evaluated using the correct signed or unsigned comparison for each funct3 variant.
- The simulator treats `0x00000000` as a stop sentinel at the end of the instruction stream.
- Loads and stores operate on 64-bit doublewords.

## Project Layout

- `src/main.cpp` - program entry point and sample test data setup
- `src/processor.cpp` - processor implementation, decode logic, execution logic, and dump helpers
- `include/processor.hpp` - processor class declaration
- `data/instructions.txt` - program image loaded by the simulator

## Building

From the `src` directory:

```bash
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude main.cpp processor.cpp -o riscv_sim
```

## Running

From the `src` directory:

```bash
./riscv_sim
```

You can also use the provided Makefile:

```bash
make run
```

## Example Workflow

1. Put your program words into `data/instructions.txt`, one 32-bit hex value per line.
2. Build and run the simulator.
3. Inspect the printed register snapshot and memory dump.

## Example Program

The current test program in `data/instructions.txt` demonstrates a bubble sort over the sample values loaded in `main.cpp`. It uses the supported arithmetic, memory, branch, and jump instructions to sort five 64-bit integers.

## Limitations

This simulator is intentionally minimal. It does not currently implement:

- `lui`
- `auipc`
- shift instructions such as `slli`, `srli`, and `srai`
- set-less-than instructions such as `slt` and `slti`
- multiply/divide instructions
- floating-point instructions
- compressed instructions

## Notes for Use

- The simulator expects instruction words in little-endian hex form in `data/instructions.txt`.
- If you want a program to terminate cleanly, end the file with `0x00000000`.
- The project is best used as a learning tool for instruction decoding, control flow, and memory access behavior.

