#include "stdint.h"
#include "mem.h"
#include <stdlib.h>

// assume read_byte(int address)
// 				write_byte(int address, uint8_t value)

typedef struct cpu_state {

	uint8_t pc;
	uint8_t sp;
	uint8_t f;

	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;

} cpu_state_t;

cpu_state_t* cpu;
mem_state_t* mem;

int cpu_init(mem_state_t* new_mem) {

	mem = new_mem;
	cpu  = malloc(sizeof(struct cpu_state));

	return 0;
}

int cpu_cleanup() {
	free(cpu);
	return 0;
}

int exec_instr() {

	unsigned int instr = mem.read_byte(cpu->pc);

	switch(instr) {

		case 0: // NOP


	}

}
