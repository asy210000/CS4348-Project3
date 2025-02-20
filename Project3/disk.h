#ifndef DISK_H
#define DISK_H

#include "scheduler.h"

int* translate(char *instruction);
int load_prog(char* fname, int addr);
void load_programs(char fname[]);
int generate_process_id();

#endif // DISK_H
