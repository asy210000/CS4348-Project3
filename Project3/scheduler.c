#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "scheduler.h"
#include "memory.h"
#include "smm.h"

struct node* readyQueue = NULL; // Definition of the global variable
PCB processTable[MAX_PROCESSES]; // Definition of the global variable
int processCount = 0;
int currentCycle = 0;
int TIME_QUANTUM = 10;

void next_process() {
    //printf("Entering next_process()");
    if (readyQueue != NULL && readyQueue->next != NULL) {
        struct node* temp = readyQueue;
        readyQueue = readyQueue->next;
        temp->next = NULL;

        struct node* lastNode = readyQueue;
        while (lastNode->next != NULL) {
            lastNode = lastNode->next;
        }
        lastNode->next = temp;
        //printf("Moved process ID %d to the back of the queue.", temp->data->processID);
    } else {
        //printf("There is only one process in the queue or it's empty.\n");
    }
}

void new_process(int base, int size) {
    printf("Trying to create a new process with base: %d, size: %d\n", base, size);
    if (processCount >= MAX_PROCESSES) {
        printf("Cannot create new process, maximum number reached.\n");
        return;
    }

    // Initialize the new process control block
    PCB *newProcess = &processTable[processCount];
    newProcess->processID = processCount;
    newProcess->Base = base;
    newProcess->PC = 0;
    newProcess->AC = 0;
    newProcess->IR0 = 0;
    newProcess->IR1 = 0;
    newProcess->MAR = 0;
    newProcess->MBR = 0;
    //printf("New process with ID %d created, Base: %d, PC set to: %d\n", processCount, newProcess->Base, newProcess->PC);

    // Add the new process to the ready queue
    struct node* newNode = (struct node*) malloc(sizeof(struct node));
    if (!newNode) {
        fprintf(stderr, "Failed to allocate memory for new process node.\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = newProcess;
    newNode->next = NULL;

    if (readyQueue == NULL) {
        readyQueue = newNode;
        printf("New process added as the head of the queue.\n");
    } else {
        struct node* lastNode = readyQueue;
        while (lastNode->next != NULL) {
            lastNode = lastNode->next;
        }
        lastNode->next = newNode;
        printf("New process added to the end of the queue.\n");
    }
    processCount++;
}

int is_process_in_queue(int processID, struct node* queue) {
    struct node* current = readyQueue;
    while (current != NULL) {
        if (current->data->processID == processID) {
            return 1; // Process found
        }
        current = current->next;
    }
    return 0; // Process not found
}

int schedule(int cycle_num, int process_status) {
    //printf("Entering schedule() - Cycle: %d, Process status: %d\n", cycle_num, process_status);

    if (readyQueue != NULL) {
        PCB *currentProcess = readyQueue->data;
        //printf("Current process ID: %d", currentProcess->processID);
        int oldProcessID = currentProcess->processID; // Keep track of current process ID
        
        register_struct old_reg_vals = {
            .Base = Base, 
            .PC = PC, 
            .AC = AC, 
            .IR0 = IR0, 
            .IR1 = IR1, 
            .MAR = MAR, 
            .MBR = MBR 
        };

        // Increment currentCycle for each cycle of the process
        currentCycle++;

        // Check if the time quantum has expired or the process has terminated
        if (currentCycle >= TIME_QUANTUM || process_status == 0) {
            if (process_status == 0) {
                printf("Process ID: %d terminated. Removing from queue.\n", currentProcess->processID);
                remove_process(&readyQueue, currentProcess->processID);
            } else {
                printf("Time quantum expired for process ID: %d\n", currentProcess->processID);
                next_process();
            }

            // Reset currentCycle for the next process
            currentCycle = 0;

            if (readyQueue != NULL) {
                PCB *nextProcess = readyQueue->data;
                register_struct new_reg_vals = {
                    .Base = nextProcess->Base,
                    .PC = nextProcess->PC,
                    .AC = nextProcess->AC,
                    .IR0 = nextProcess->IR0,
                    .IR1 = nextProcess->IR1,
                    .MAR = nextProcess->MAR,
                    .MBR = nextProcess->MBR
                };
                old_reg_vals = context_switch(new_reg_vals);
            }
        }

        if (is_process_in_queue(oldProcessID, readyQueue)) {
            // Update PCB of the old process
            currentProcess->Base = old_reg_vals.Base;
            currentProcess->PC = old_reg_vals.PC;
            currentProcess->AC = old_reg_vals.AC;
            currentProcess->IR0 = old_reg_vals.IR0;
            currentProcess->IR1 = old_reg_vals.IR1;
            currentProcess->MAR = old_reg_vals.MAR;
            currentProcess->MBR = old_reg_vals.MBR;
        }
    } else {
        // If the queue is empty, reset the cycle counter
        currentCycle = 0;
    }

    //printf("Schedule function exiting");
    return readyQueue != NULL;
}

void remove_process(struct node** head, int processID) {
    //printf("Attempting to remove process with ID %d", processID);
    struct node* temp = *head;
    struct node* prev = NULL;

    if (temp != NULL && temp->data->processID == processID) {
        *head = temp->next; // Move the head pointer to next node
        free(temp); // Free the memory of the node
        printf("Process with ID %d removed from the head of the queue.\n", processID);
        return;
    }

    while (temp != NULL && temp->data->processID != processID) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Process with ID %d not found in the queue.", processID);
        return;
    }

    prev->next = temp->next; // Unlink the node from the list
    free(temp); // Free the memory of the node
    printf("Process with ID %d removed from the queue.\n", processID);
}

int get_current_process_pid() {
    if (readyQueue != NULL) {
        return readyQueue->data->processID;
    } else {
        return -1; // Indicates that there is no process currently running
    }
}
