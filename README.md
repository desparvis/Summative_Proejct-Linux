# Summative Programming Projects: Linux Programming

### Author: Credo Desparvis Gutabarwa
### Date: April 2026
### Environment: Linux / WSL (Ubuntu)

## Project Structure

Project/<br>
│<br>
├── question1/<br>
│   ├── program.c<br>
│   ├── program (stripped ELF)<br>
│   └── report.md<br>
│<br>
├── question2/<br>
│   └── q2.asm<br>
│<br>
├── question3/<br>
│   ├── vibration.c<br>
│   ├── setup.py<br>
│   └── test_vibration.py<br>
│<br>
├── question4/<br>
│   └── q4.c<br>
│<br>
├── question5/<br>
│   ├── server.c<br>
│   └── client.c<br>

# Compilation & Execution Guide

## Question 1: ELF Reverse Engineering

Compiles and analyzes a C program at multiple system levels.

cd question1

gcc -Wall -O0 -fno-inline -o program program.c
strip program

### Static analysis
readelf -h program
readelf -S program
objdump -d program

### Runtime analysis
strace ./program
gdb ./program

## Question 2: x86 Assembly

Processes temperature_data.txt to count total and valid readings. Handles LF and CRLF line endings.

cd question2

nasm -f elf64 q2.asm -o q2.o
ld q2.o -o q2
./q2

## Question 3: Python C Extension

High-speed vibration analysis module using Welford’s Algorithm.

cd question3

python3 setup.py build_ext --inplace
python3 test_vibration.py

## Question 4: Multithreaded Baggage System

Simulates a Producer-Consumer model with a circular buffer.

cd question4

gcc q4.c -o q4 -lpthread
./q4

## Question 5: TCP Library Server

Concurrent reservation system using sockets and threads.

cd question5

gcc server.c -o server -lpthread
gcc client.c -o client

### Run server
./server

### Run client (separate terminal)
./client

# Technical Documentation Summary

## 1. Reverse Engineering (ELF Binary Analysis)

Program Design:
Implements three user-defined functions, a loop, a conditional branch, dynamic memory allocation (malloc), and a global variable stored in .data.

ELF Analysis:

Architecture: x86-64
Entry point: _start
Sections:
.text → executable code
.data → initialized globals
.bss → uninitialized data
.plt/.got → dynamic linking

Disassembly:
Control flow reconstructed using objdump, identifying:

Loop via backward jumps
Conditional logic via cmp and branch instructions

System Calls:
Using strace:

execve → program start
write → output (printf)
brk/mmap → heap allocation (malloc)

Runtime Analysis:
Using gdb:

Stack → local variables
Heap → dynamic allocation
Global → .data/.bss

Advanced Insight:

Symbol stripping removes function names
System V ABI uses registers (RDI, RSI) for arguments
Dynamic linking handled via PLT/GOT

Complexity:
Time complexity is O(n) based on loop traversal.

## 2. Low-Level Processing (x86 Assembly)

Optimization:
Loads data into a buffer to minimize system calls.

Traversal Logic:
Processes file character-by-character, detecting newlines (\n) and tracking non-empty lines.

Robustness:
Handles both LF and CRLF line endings by skipping \r.

Error Handling:
Validates file descriptor after sys_open.

Complexity:
O(n) where n is the number of bytes processed.

## 3. Industrial Analytics (C Extension)

Numerical Accuracy:
Uses Welford’s Algorithm for stable variance computation.

Memory Strategy:
Avoids unnecessary allocations and uses proper reference counting.

Efficiency:
Single-pass computation with O(n) complexity.

## 4. Concurrency & Synchronization (Baggage System)

Mechanism:
Uses mutexes and condition variables for synchronization.

Logic:
Circular buffer prevents overflow and ensures bounded resource usage.

Thread Safety:
Monitoring thread safely reads shared data periodically.

## 5. Distributed Systems (TCP Socket Server)

Protocol:
Structured message format ensures reliable communication.

Concurrency:
Thread-per-client model with automatic cleanup.

Integrity:
Mutex-protected operations prevent race conditions (e.g., double booking).

## Defensive Programming & Robustness

Each project includes:

Input Validation: Checks for invalid or missing data
Graceful Termination: Handles EOF, socket closure, and errors cleanly
Resource Management: Proper cleanup of memory, file descriptors, and threads