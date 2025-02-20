#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "memory.h"
#include "scheduler.h"
#include "disk.h"
#include "smm.h"

int* translate(char* instruction) {
    if (instruction == NULL) {
        fprintf(stderr, "translate() received a NULL instruction pointer.\n");
        exit(EXIT_FAILURE);
    }

    // Duplicate the instruction to avoid modifying the original
    char* tempInstruction = strdup(instruction); 
    if (tempInstruction == NULL) {
        fprintf(stderr, "Failed to duplicate the instruction for translation.\n");
        exit(EXIT_FAILURE);
    }

    char* command = strtok(tempInstruction, "\n \r");
    char* argument = strtok(NULL,"\n \r");

    if (command == NULL) {
        fprintf(stderr, "No command found in the instruction.\n");
        free(tempInstruction);
        exit(EXIT_FAILURE);
    }

    // Allocate memory for translation
    int* translation = malloc(2 * sizeof(int)); 
    if (translation == NULL) {
        fprintf(stderr, "Failed to allocate memory for translation.\n");
        free(tempInstruction);
        exit(EXIT_FAILURE);
    }

    if (strcmp(command, "load_const") == 0 || strcmp(command, "ifgo") == 0) {
        if (argument == NULL) {
            fprintf(stderr, "[ERROR] Missing argument for command: %s\n", command);
            free(tempInstruction);
            free(translation);
            return NULL;
        }
    }
    translation[0] = -1; // Initialize as unknown or comment
    translation[1] = 0;

    // Begin command translation
    if (strncmp(command, "//", 2) == 0) {
        translation[0] = -1; // Comment line, ignore
    } else if (strcmp(command, "exit") == 0) {
        translation[0] = 0;
        translation[1] = 0;
    } else if (strcmp(command, "load_const") == 0) {
        translation[0] = 1;
        translation[1] = argument ? atoi(argument) : 0;
    } else if (strcmp(command, "move_from_mbr") == 0) {
        translation[0] = 2;
    } else if (strcmp(command, "move_from_mar") == 0) {
        translation[0] = 3;
    } else if (strcmp(command, "move_to_mbr") == 0) {
        translation[0] = 4;
    } else if (strcmp(command, "move_to_mar") == 0) {
        translation[0] = 5;
    } else if (strcmp(command, "load_at_addr") == 0) {
        translation[0] = 6;
    } else if (strcmp(command, "write_at_addr") == 0) {
        translation[0] = 7;
    } else if (strcmp(command, "add") == 0) {
        translation[0] = 8;
    } else if (strcmp(command, "multiply") == 0) {
        translation[0] = 9;
    } else if (strcmp(command, "and") == 0) {
        translation[0] = 10;
    } else if (strcmp(command, "or") == 0) {
        translation[0] = 11;
    } else if (strcmp(command, "ifgo") == 0) {
        translation[0] = 12;
        translation[1] = argument ? atoi(argument) : 0;
    } else if (strcmp(command, "sleep") == 0) {
        translation[0] = 13;
    } else {
        // Handle unknown command
        if (translation[0] == -1 && command[0] != '/') {
        fprintf(stderr, "Unknown command: %s\n", command);
        free(tempInstruction);
        free(translation);
        return NULL;
        }
    }
    free(tempInstruction);
    return translation;
}

int load_prog(char* fname, int addr) {
    FILE* file_pointer = fopen(fname, "r");
    if (!file_pointer) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int prog_size = 0; // Counter for the size of the program

    while ((read = getline(&line, &len, file_pointer)) != -1) {
        int* translation = translate(line);

        if (translation[0] != -1) {
            mem_write(addr, translation);
            addr++;
            prog_size++; // Increment program size for each instruction written
        } else {
            printf("Skipped line: %s", line); // Log skipped line
        }

        free(translation);
    }

    //printf("Finished loading program '%s', size: %d\n", fname, prog_size);
    free(line);
    fclose(file_pointer);
    return prog_size; // Return the size of the program
}

void load_programs(char fname[]) {
    FILE *file = fopen(fname, "r");
    if (file == NULL) {
        perror("Error opening program list file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    int requested_size;
    char program_filename[100];
    int actual_size;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d %s", &requested_size, program_filename) == 2) {
            int pid = generate_process_id(); // Generate a unique process ID
            int allocation_status = allocate(pid, requested_size); // Request memory allocation

            if (allocation_status == 1) {
                int base_address = get_base_address(pid); 
                actual_size = load_prog(program_filename, base_address);
                
                // Create and add the PCB to the ready queue
                new_process(base_address, actual_size);
            } else {
                printf("Memory allocation failed for program %s\n", program_filename);
            }
        }
    }
    fclose(file);
}


// Function to generate a unique process ID
int generate_process_id() {
    static int current_pid = 0;

    // Increment the current PID
    // Wrap around if it reaches the maximum value of an integer
    if (current_pid == INT_MAX) {
        current_pid = 0;
    } else {
        current_pid++;
    }

    return current_pid;
}


