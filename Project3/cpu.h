#ifndef CPU_H
#define CPU_H

extern int PC;  // Program Counter
extern int AC;  // Accumulator Register
extern int Base;  // Base Register
extern int IR0;   // Instruction Register 0 (OP Code)
extern int IR1;   // Instruction Register 1 (Argument)
extern int MAR;   // Memory Address Register
extern int MBR;   // Memory Buffer Register

typedef struct register_struct {
    int Base;
    int PC;
    int IR0;
    int IR1;
    int AC;
    int MAR;
    int MBR;
} register_struct;

register_struct context_switch(register_struct new_state);
void fetch_instruction(int addr);
void execute_instruction();
int mem_address(int l_addr);
int clock_cycle();

#endif // CPU_H
