#include <stdio.h>

#include "disk.h"
#include "memory.h"
#include "cpu.h"
#include "scheduler.h"
#include "smm.h"

int main() {
    char fname[] = "program_list_invalid_access.txt"; // file path
    int running = 1;
    int cycle_count = 0;

    // Initialize the system and load the programs
    load_programs(fname);

    if (readyQueue != NULL) {
        PCB *firstProcess = readyQueue->data;

        // Set the CPU's state for the first process
        Base = firstProcess->Base;
        PC = firstProcess->PC;
        //printf("Starting first process with Base: %d, PC: %d\n", Base, PC);
    }

    // Main scheduling loop
    while (running && readyQueue != NULL) {  // Check that the ready queue is not empty
        printf("Cycle count: %d\n", cycle_count);
        int clock_ret = clock_cycle(); // Simulate a clock cycle
        running = schedule(cycle_count, clock_ret); // Determine if we should continue running
        cycle_count++;
    }

    // After the scheduling is done
    printf("All processes have completed or the ready queue is empty.\n");
    
    // Print the specified memory locations
    int specified_locations[] = {30, 150, 230};
    int i;
    for (i = 0; i < 3; i++) {
        int addr = specified_locations[i];
        int* data = mem_read(addr);
        if(data != NULL) {
            printf("Memory[%d] - Opcode: %d Argument: %d\n", addr, data[0], data[1]);
        } else {
            printf("Memory[%d] - Empty or inaccessible\n", addr);
        }
    }

    // Print the number of memory holes created
    printf("Number of memory holes created: %d\n", hole_count);

    return 0;
}
