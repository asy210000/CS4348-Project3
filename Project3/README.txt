READ ME: CS-4348 Project 3 - Simple Hardware Simulator with Memory Management
Name: Austin Yatco
NetID: 2021616010

QUICK START:
1. Adjust config settings in main.c as described in the CONFIG section.
2. Run make to compile all .c files.
3. Execute with ./sim.exe.

DESCRIPTION:
In Project 3, we extend the functionality of our previous project by adding memory management capabilities. 
Each process is now assigned a distinct memory space and is restricted to read/write operations within that space. 
Memory spaces are non-overlapping, ensuring process isolation. This project simulates an operating system's memory management scheme.

FILES:
1. disk.c - Manages program loading based on allocation requests. Works with `smm.c` for memory space checks.
2. smm.c - Implements the Simple Memory Manager (SMM), handling memory allocation and deallocation using dynamic partitioning.
3. memory.c - Altered mem_read() and mem_write() functions to integrate with SMM for operation validation.
4. cpu.c - Simulates the execution of instructions stored in memory; fetches, decodes, and executes the instructions.
5. scheduler.c - Adds functions for process removal from the ready queue and retrieval of the current process's PID.
6. main.c - Main execution file, with system configuration and program execution simulation.
7. Header files - provides declarations for functions, constants, and data structures used across different source files. (cpu.h, disk.h, memory.h, scheduler.h, smm.c)
8. Makefile - Used for compiling all of the source files together.

CONFIG:
1. **File Paths**: On line 10 in main.c file, make sure the file name is set to correct path for "program_list_invalid_access.txt.txt". (either program_list_valid.txt, program_list_invalid_allocation.txt, or program_list_invalid_access.txt.)
2. **Instruction Set Files**: Confirm that all program files listed in program_list_xxx.txt, such as loop50.txt, loop100.txt, loop200_valid.txt, and loop200_invalid.txt, are located in the same directory as the executable.
3. **Memory Allocation File**: Make sure the program_list_xxx.txt properly enumerates the memory size required for each program and the associated filenames, formatted as [Memory Size] [Filename].

HOW TO RUN:
1. Use the Makefile I've created to compile all the source files (disk.c, memory.c, cpu.c, scheduler.c, smm.c, main.c) and run it by typing "make".
2. It should create an executable named "sim.exe". Run it by typing "./sim"
or
3. Manually compile with gcc by entering this command - `gcc disk.c memory.c cpu.c scheduler.c smm.c main.c -o sim`
4. Then, run it by typing `./sim`