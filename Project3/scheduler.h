#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "cpu.h"  // for register_struct type
#include "smm.h"

typedef struct PCB {
    int processID; // Unique identifier for the process
    int Base;      // Base register value
    int PC;        // Program Counter
    int IR0;       // Instruction Register 0 (OP Code)
    int IR1;       // Instruction Register 1 (Argument)
    int AC;        // Accumulator Register
    int MAR;       // Memory Address Register
    int MBR;       // Memory Buffer Register
    int size;      // Size of the process in memory (number of instructions)
} PCB;

typedef struct node {
    PCB *data;
    struct node *next;
} node;

extern struct node* readyQueue;
extern PCB processTable[MAX_PROCESSES];
extern int processCount;
extern int currentCycle;

void next_process();
void new_process(int base, int size);
void remove_process(struct node** head, int processID);
int schedule(int cycle_num, int process_status);
int get_current_process_pid();
#endif // SCHEDULER_H
