#include "stdint.h"

typedef mem_state_t;

mem_state_t* init_mem();
int cleanup_mem(mem_state_t* mem);

int read_byte(mem_state_t* mem, int addr);
int write_byte(mem_state_t* mem, int addr, int val);
