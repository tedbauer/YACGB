#include "stdint.h"
#include "string.h"
#include "mem.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum {
	/* 16 bit registers: */
	PC=0, SP=2,

	/* 8 bit registers: */
	A=4, B=5, C=6, D=7, E=8,
	F=9, H=10, L=11,

	/* Paired registers: */
	AF, BC, DE, HL
} reg_t;

typedef struct cpu_state {
	uint8_t registers[12];
	unsigned int lc;
} cpu_state_t;

cpu_state_t* cpu;
mem_state_t* mem;

typedef enum {
	BIT8,
	BIT16
} mem_t_id;

typedef union {
	uint8_t bit8;
	uint16_t bit16;
	mem_t_id tid;
} mem_t;

/* Place contents of new_val in register r. */
void write_reg(reg_t r, mem_t new_val) {
	switch(r) {
		case PC:
		case SP:
			cpu->registers[r] = new_val.bit16; break;
		case AF:
			cpu->registers[A] = new_val.bit16; break;
		case BC:
			cpu->registers[B] = new_val.bit16; break;
		case DE:
			cpu->registers[D] = new_val.bit16; break;
		case HL:
			cpu->registers[H] = new_val.bit16; break;
		default:
			cpu->registers[r] = new_val.bit8;
	}
}

/* FIXME i think getting the 16bit vals is wrong */
mem_t read_preg(reg_t r) {
	mem_t result;
	switch(r) {
		case AF:
			result.bit16 = cpu->registers[A];
			break;
		case BC:
			result.bit16 = cpu->registers[B];
			break;
		case DE:
			result.bit16 = cpu->registers[D];
			break;
		case HL:
			result.bit16 = cpu->registers[H];
			break;
		default:
			perror("Non-pair supplied to read_preg.");
	}
	return result;
}

/* FIXME i think getting the 16bit vals is wrong */
mem_t read_reg(reg_t r) {
	mem_t result;
	switch(r) {
		case PC:
			result.bit16 = cpu->registers[r];
			break;
		case SP:
			result.bit16 = cpu->registers[r];
			break;
		case AF:
		case BC:
		case DE:
		case HL:
			return read_preg(r);
		default:
			result.bit8 = cpu->registers[r];
	}

	return result;
}

mem_t read_nn() {
	perror("Implement me!");
	mem_t bad;
	return bad;
}

mem_t read_n() {
	mem_t result;
	result.bit8 = mem.read_byte(pc+1);
	return result;
}

void set_lclock(int lclock) {
	cpu->lc = lclock;
}

int cpu_init(mem_state_t* new_mem) {
	mem = new_mem;
	cpu  = malloc(sizeof(struct cpu_state));
	return 0;
}

int cpu_cleanup() {
	free(cpu);
	return 0;
}

mem_t incr(mem_t val) {
	mem_t result = val;
	switch(val.tid) {
		case BIT8:
			result.bit8 += 1;
		case BIT16:
			result.bit16 += 1;
		default:
			perror("incr call failed.");
	}
	return result;
}

/* TODO (maybe): combine with incr? */
mem_t decr(mem_t val) {
	mem_t result = val;
	switch(val.tid) {
		case BIT8:
			result.bit8 -= 1;
		case BIT16:
			result.bit16 -= 1;
		default:
			perror("decr call failed.");
	}
	return result;
}

#define INSTR_TABLE(F) \
	F(0x00, 4,  asm("nop"))                        /* NOP        */ \
	F(0x01, 12, write_reg(BC, read_nn()))          /* LD BC, nn  */ \
	F(0x02, 8,  write_reg(BC, read_reg(A)))        /* LD (BC), A */ \
	F(0x03, 8,  write_reg(BC, incr(read_reg(BC)))) /* INC BC     */ \
	F(0x04, 4,  write_reg(B, incr(read_reg(B))))   /* INC B      */ \
	F(0x05, 4,  write_reg(B, decr(read_reg(B))))   /* DEC B      */ \
	F(0x06, 8,  write_reg(B, read_n()))            /* LD B, n    */ \

#define INSTR_FUNCS(OP, CYCLES, CMDS) void exec##OP() \
	{ CMDS; set_lclock(CYCLES); }
#define INSTR_FUNCNAMES(OP, CYCLES, CMDS)  exec##OP,

INSTR_TABLE(INSTR_FUNCS)
void (*instructions[])() = { INSTR_TABLE(INSTR_FUNCNAMES) };

#define EXEC_INSTR(instr) instructions[instr]()

int step_cpu() {
	char* instr = mem.read_byte(cpu->pc);
	EXEC_INSTR((uint8_t)instr);
}
