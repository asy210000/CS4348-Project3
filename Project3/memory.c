#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "smm.h"
#include "scheduler.h"

// Defining the memory as a 2D array
int memory[1024][2];

// Function to read from the memory
int* mem_read(int addr) {
    int current_pid = get_current_process_pid();
    if (!is_allowed_address(current_pid, addr)) {
        fprintf(stderr, "Memory access violation by process %d at address %d.\n", current_pid, addr);
        deallocate(current_pid); // Deallocate memory for this process
        remove_process(&readyQueue, current_pid); // Remove process from the ready queue
        return NULL; // Return NULL to indicate the failure
    }

    if (addr < 0 || addr >= 1024) {
        fprintf(stderr, "Error: Memory address out of bounds.\n");
        exit(EXIT_FAILURE);
    }
    
    //printf("Memory read from address %d: [%d, %d]\n", addr, memory[addr][0], memory[addr][1]);
    return memory[addr];
}

void mem_write(int addr, int* data) {
    int current_pid = get_current_process_pid();
    if (!is_allowed_address(current_pid, addr)) {
        fprintf(stderr, "Memory access violation by process %d at address %d.\n", current_pid, addr);
        deallocate(current_pid); // Deallocate memory for this process
        remove_process(&readyQueue, current_pid); // Remove process from the ready queue
        return; // Return early to indicate the failure
    }

    if (addr < 0 || addr >= 1024) {
        fprintf(stderr, "Error: Memory address fout of bounds.\n");
        exit(EXIT_FAILURE);
    }

    memory[addr][0] = data[0]; // Write the opcode
    memory[addr][1] = data[1]; // Write the operand
    
    //printf("Memory write to address %d: [Opcode: %d, Operand: %d]\n", addr, data[0], data[1]);
}
