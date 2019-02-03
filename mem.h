#include "stdint.h"

typedef mem_state_t;

mem_state_t* mem_init();
int mem_cleanup(mem_state_t* mem);

int read_byte(mem_state_t* mem, uint8_t addr);
int write_byte(mem_state_t* mem, uint8_t addr, uint8_t val);
