#ifndef SMM_H
#define SMM_H

#define MAX_PROCESSES 256

// Assuming you have a global variable for counting memory holes
extern int hole_count;

// Function prototypes
void init_allocation_table();  // Initializes the allocation table
int allocate(int pid, int size);  // Allocates memory for a process
void deallocate(int pid);  // Deallocates memory of a process
void add_hole(int base, int size);  // Adds a memory hole
void remove_hole(int base);  // Removes a memory hole
void merge_holes();  // Merges adjacent memory holes
int find_hole(int size);  // Finds a hole using the First-Fit algorithm
int get_base_address(int pid);  // Gets the base address for a process
int find_empty_row();  // Finds an empty row in the allocation table
int is_allowed_address(int pid, int addr);  // Checks if an address is allowed for a process

#endif // SMM_H
