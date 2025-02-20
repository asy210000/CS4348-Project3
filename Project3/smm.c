#include <stdio.h>
#include <stdlib.h>
#include "smm.h"
#include "scheduler.h"

int hole_count = 0;

// Structure for memory holes
typedef struct MemoryHole {
    int base_address;
    int size;
    struct MemoryHole *next;
} MemoryHole;

// Structure for the allocation table
typedef struct {
    int pid;
    int base_address;
    int size;
} AllocationEntry;

// Global variables
MemoryHole *holes_head = NULL;
AllocationEntry allocation_table[MAX_PROCESSES];

// Function to initialize the allocation table
void init_allocation_table() {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        allocation_table[i].pid = -1;
        allocation_table[i].base_address = 0;
        allocation_table[i].size = 0;
    }
}

// Function to find a hole
int find_hole(int size) {
    MemoryHole *current = holes_head;
    while (current != NULL) {
        if (current->size >= size) {
            return current->base_address;
        }
        current = current->next;
    }
    return -1;
}

// Function to find an empty row in the allocation table
int find_empty_row() {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (allocation_table[i].size == 0) {
            return i;
        }
    }
    return -1; // No empty row found
}

// Function to add a hole
void add_hole(int base, int size) {
    MemoryHole *new_hole = (MemoryHole *)malloc(sizeof(MemoryHole));
    if (!new_hole) {
        // Handle memory allocation failure
        fprintf(stderr, "Failed to allocate memory for new hole.\n");
        return;
    }

    new_hole->base_address = base;
    new_hole->size = size;
    new_hole->next = NULL;

    // Add the new hole to the list
    if (holes_head == NULL || holes_head->base_address > base) {
        new_hole->next = holes_head;
        holes_head = new_hole;
    } else {
        MemoryHole *current = holes_head;
        while (current->next != NULL && current->next->base_address < base) {
            current = current->next;
        }
        new_hole->next = current->next;
        current->next = new_hole;
    }

    hole_count++; // Increment the hole count
    merge_holes();
}

// Function to remove a hole
void remove_hole(int base) {
    if (holes_head == NULL) {
        return;
    }

    MemoryHole *current = holes_head;
    MemoryHole *prev = NULL;

    while (current != NULL) {
        if (current->base_address == base) {
            if (prev == NULL) {
                // The hole to remove is the first in the list
                holes_head = current->next;
            } else {
                // The hole to remove is not the first
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Function to merge adjacent holes
void merge_holes() {
    MemoryHole *current = holes_head;
    while (current != NULL && current->next != NULL) {
        if (current->base_address + current->size == current->next->base_address) {
            current->size += current->next->size;
            MemoryHole *temp = current->next;
            current->next = temp->next;
            free(temp);
            hole_count++; // Increment the hole count due to modification
        } else {
            current = current->next;
        }
    }
}

// Function to allocate memory
int allocate(int pid, int size) {
    int base_address = find_hole(size);
    if (base_address == -1) {
        return 0; // No suitable hole found
    }

    int empty_row = find_empty_row();
    if (empty_row == -1) {
        return 0; // Allocation table is full
    }

    // Update allocation table
    allocation_table[empty_row].pid = pid;
    allocation_table[empty_row].base_address = base_address;
    allocation_table[empty_row].size = size;
    
    // Update hole list
    MemoryHole *current = holes_head;
    MemoryHole *prev = NULL;
    while (current != NULL) {
        if (current->base_address == base_address) {
            if (current->size > size) {
                current->base_address += size;
                current->size -= size;
            } else if (current->size == size) {
                remove_hole(current->base_address);
            }
            break;
        }
        prev = current;
        current = current->next;
    }

    return 1; // Memory allocated successfully
}

// Function to deallocate memory
void deallocate(int pid) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (allocation_table[i].pid == pid) {
            add_hole(allocation_table[i].base_address, allocation_table[i].size);
            allocation_table[i].pid = -1;
            allocation_table[i].size = 0;
            break;
        }
    }
}

// Function to check if an address is allowed for a process
int is_allowed_address(int pid, int addr) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (allocation_table[i].pid == pid) {
            if (addr >= allocation_table[i].base_address && addr < allocation_table[i].base_address + allocation_table[i].size) {
                return 1;
            }
            break;
        }
    }
    return 0;
}

// Function to get the base address for a process
int get_base_address(int pid) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (allocation_table[i].pid == pid) {
            return allocation_table[i].base_address;
        }
    }
    return -1; // PID not found
}
