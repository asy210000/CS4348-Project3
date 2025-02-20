#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "memory.h"
#include "scheduler.h"

// Registers
int Base = 0;  // Base Register
int PC = 0;    // Program Counter
int IR0 = 0;   // Instruction Register 0 (OP Code)
int IR1 = 0;   // Instruction Register 1 (Argument)
int AC = 0;    // Accumulator Register
int MAR = 0;   // Memory Address Register
int MBR = 0;   // Memory Buffer Register

register_struct context_switch(register_struct new_vals) {
    register_struct old_vals;
    // Save the current register values
    old_vals.Base = Base;
    old_vals.PC = PC;
    old_vals.IR0 = IR0;
    old_vals.IR1 = IR1;
    old_vals.AC = AC;
    old_vals.MAR = MAR;
    old_vals.MBR = MBR;

    Base = new_vals.Base;
    PC = new_vals.PC;
    IR0 = new_vals.IR0;
    IR1 = new_vals.IR1;
    AC = new_vals.AC;
    MAR = new_vals.MAR;
    MBR = new_vals.MBR;

    return old_vals;
}

void execute_instruction() {
    switch(IR0) {
        case 0: // exit
            break;

        case 1: // load_const
            AC = IR1;
            break;

        case 2: // move_from_mbr
            AC = MBR;
            break;

        case 3: // move_from_mar
            AC = MAR;
            break;

        case 4: // move_to_mbr
            MBR = AC;
            break;

        case 5: // move_to_mar
            MAR = AC;
            break;

        case 6: // load_at_addr
            {
            int* load_result = mem_read(MAR);
            MBR = load_result[1];
            break;
            }

        case 7: // write_at_addr
            {
            int* current_memory = mem_read(MAR);
            int data_to_write[2] = {current_memory[0], MBR}; // Keep current opcode, update operand with MBR
            mem_write(MAR, data_to_write);
            break;
            }

        case 8: // add
            AC += MBR;
            break;

        case 9: // multiply
            AC *= MBR;
            break;

        case 10: // and
            AC = AC && MBR ? 1 : 0;
            break;

        case 11: // or
            AC = AC || MBR ? 1 : 0;
            break;

        case 12: // ifgo
            //printf("ifgo - Conditional jump if AC is not 0. AC: %d, Jump to: %d\n", AC, IR1 - 1);
            if (AC != 0) {
                PC = IR1 - 1;
                //printf("Jumping to: %d\n", PC);
            }
            break;

        case 13: // sleep
            // Do nothing. Just advance the program counter.
            break;

        default:
            break;
    }
    //printf("Post Execution - AC: %d, MAR: %d, MBR: %d, PC: %d\n", AC, MAR, MBR, PC);
}

void fetch_instruction(int addr) {
    int* instruction = mem_read(addr);
    IR0 = instruction[0];  // Opcode
    IR1 = instruction[1];  // Argument
}

int mem_address(int l_addr) {
    return Base + l_addr;
}

int clock_cycle() {
    fetch_instruction(mem_address(PC));  // Fetch instruction from current PC relative to Base
    execute_instruction();  // Execute the fetched instruction
    PC++;
    // If exit opcode is encountered, return 0
    if (IR0 == 0) {
        return 0;
    }
    return 1;
}