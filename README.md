Summative Programming Projects: Linux Programming

Author: Credo Desparvis Gutabarwa

Date: April 2026

Environment: Linux / WSL (Ubuntu)

Project StructureFolderContentsDescriptionquestion2/q2.asmx86 Assembly Temperature Data Processorquestion3/vibration.c, setup.py, test_vibration.pyPython C Extension for Industrial Analyticsquestion4/q4.cPOSIX Multithreaded Baggage Systemquestion5/server.c, client.cTCP Concurrent Library Reservation System🛠 Compilation & Execution GuideQuestion 2: x86 AssemblyProcesses temperature_data.txt to count total and valid readings. Handles LF and CRLF line endings.Bashcd question2

nasm -f elf32 q2.asm -o q2.o
ld -m elf_i386 q2.o -o q2
./q2

Question 3: Python C ExtensionHigh-speed vibration analysis module using Welford’s Algorithm for numerical stability.Bashcd question3
python3 setup.py build_ext --inplace
python3 test_vibration.py
Question 4: Multithreaded Baggage SystemSimulates a Producer-Consumer model with a Circular Buffer and a real-time Monitor Thread.Bashcd question4
gcc q4.c -o q4 -lpthread
./q4
Question 5: TCP Library ServerA concurrent, authenticated reservation platform using a Structured Message Protocol.Bashcd question5
gcc server.c -o server -lpthread
gcc client.c -o client
# Run server in one terminal:
./server
# Run client in another:
./client
📝 Technical Documentation Summary1. Low-Level Processing (x86 Assembly)Optimization: Loads data into a 4KB buffer to minimize system calls.Robustness: Implements a state-machine traversal to skip \r (Carriage Return) characters, ensuring cross-platform compatibility.Error Handling: Validates file descriptors immediately after the sys_open interrupt.2. Industrial Analytics (C Extension)Numerical Accuracy: Uses a single-pass variance calculation to prevent floating-point precision loss.Memory Strategy: Zero heap allocations. Utilizes Py_DECREF for strict reference counting, ensuring the module has no memory leaks.Efficiency: Operates at $O(n)$ time complexity using double precision as required.3. Concurrency & Synchronization (Baggage System)Mechanism: Uses pthread_mutex_t and pthread_cond_t (condition variables) to prevent race conditions.Logic: The Circular Buffer prevents overflow beyond the 5-item capacity.Thread Safety: The Monitoring thread performs synchronized reads of shared variables every 5 seconds without stalling the Producer/Consumer pipeline.4. Distributed Systems (TCP Socket Server)Protocol: A custom Payload Struct with enums ensures a robust communication contract between client and server.Concurrency: Uses a Thread-per-Client architecture with pthread_detach for automatic resource reclamation upon disconnection.Integrity: Mutex-locked reservation logic makes "double-booking" mathematically impossible.🛡️ Defensive Programming & RobustnessEach project in this repository includes:Input Validation: Checking for null pointers, empty lists, or unauthorized IDs.Graceful Termination: Handling EOF, socket closure, or simulation completion without hanging.Resource Management: Proper closing of file descriptors and freeing of dynamically allocated memory.