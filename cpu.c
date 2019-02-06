#include "stdint.h"
#include "string.h"
#include "mem.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define TRUE  1
#define FALSE 0

typedef enum {
	/* 16 bit registers: */
	PC=0, SP=2,

	/* 8 bit registers: */
	A=4, B=5, C=6, D=7, E=8,
	F=9, H=10, L=11,

	/* Paired registers: */
	AF, BC, DE, HL
} reg_t;

typedef enum {
	fC=0, /* Carry */
	fH=1, /* Half Carry */
	fN=2, /* Subtract */
	fZ=3, /* Zero */
} flag_t;

typedef struct {
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
		case A:
		case B:
		case C:
		case D:
		case E:
		case F:
		case H:
		case L:
			cpu->registers[r] = new_val.bit8; break;
		default:
			perror("Write_reg call failed.");
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

mem_t lift(unsigned int val) {
	assert(val > 0);
	assert(val < 655356);
	mem_t result;
	if (val > 255) {
		result.bit16 = (uint16_t) val;
		result.tid   = BIT16;
	} else {
		result.bit8 = (uint8_t) val;
		result.tid  = BIT8;
	}
	return result;
}

unsigned int add(unsigned int val1, unsigned int val2) { return val1 + val2; }
unsigned int sub(unsigned int val1, unsigned int val2) { return val1 - val2; }
unsigned int xor(unsigned int val1, unsigned int val2) { return val1 ^ val2; }
unsigned int or(unsigned int val1, unsigned int val2)  { return val1 | val2; }
unsigned int lshift(unsigned int val1, unsigned int val2)  { return val1 << val2; }

unsigned int eq(unsigned int val1, unsigned int val2)  { return val1 == val2; }

/* Instead of "demoting", grab bottom bits? */
mem_t binop(unsigned int (*op)(unsigned int, unsigned int), mem_t val1, mem_t val2) {
	mem_t result;
	if (val1.tid == BIT8 && val2.tid == BIT8) {
		result.bit8 = (uint8_t) op((unsigned int)val1.bit8, (unsigned int)val2.bit8);
		result.tid  = BIT8;
	} else if (val1.tid == BIT8 && val2.tid == BIT16) {
		result.bit16 = (uint8_t) op((unsigned int)val1.bit8, (unsigned int)val2.bit16);
		result.tid  = BIT16;
	} else if (val1.tid == BIT16 && val2.tid == BIT8) {
		result.bit16 = (uint8_t) op((unsigned int)val1.bit16, (unsigned int)val2.bit8);
		result.tid  = BIT16;
	} else {
		result.bit16 = (uint8_t) op((unsigned int)val1.bit16, (unsigned int)val2.bit16);
		result.tid  = BIT16;
	}
	return result;
}

/* Instead of "demoting", grab bottom bits? */
unsigned int bool_op(unsigned int (*op)(unsigned int, unsigned int), mem_t val1, mem_t val2) {
	if (val1.tid == BIT8 && val2.tid == BIT8) {
		return op((unsigned int)val1.bit8, (unsigned int)val2.bit8);
	} else if (val1.tid == BIT8 && val2.tid == BIT16) {
		return op((unsigned int)val1.bit8, (unsigned int)val2.bit16);
	} else if (val1.tid == BIT16 && val2.tid == BIT8) {
		return op((unsigned int)val1.bit16, (unsigned int)val2.bit8);
	} else {
		return op((unsigned int)val1.bit16, (unsigned int)val2.bit16);
	}
}

unsigned int if_carry(int bit, mem_t val1, mem_t val2) {
	perror("Implement me!");
	return 0;
}

#define ADD(val1, val2)    binop(sum, val1, val2)
#define SUB(val1, val2)    binop(sub, val1, val2)
#define INCR(val)          binop(add, val, lift(1))
#define DECR(val)          binop(add, val, lift(-1))
#define XOR(val1, val2)    binop(xor, val1, val2)
#define OR(val1, val2)     binop(or, val1, val2)
#define LSHIFT(val1, val2) binop(lshift, val1, val2)
#define EQUALS(val1, val2) bool_op(eq, val1, val2)

void write_f(flag_t f, int val, int guard) {
	if (guard) {
		write_reg(F, OR(read_reg(F), LSHIFT(lift(1), SUB(lift(4), lift(f)))));
	}
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
	cpu  = malloc(sizeof(cpu_state_t)); /* FIXME: can you do this? */
	return 0;
}

int cpu_cleanup() {
	free(cpu);
	return 0;
}

void write_mem(mem_t addr, mem_t val) {
	assert(addr.tid == BIT16);
	assert(val.tid == BIT8);
	perror("Implement me!");
}

#define INSTR_TABLE(F)                                                         \
	F(0x00, 4, /* NOP */                                                       \
			asm("nop"))                                                        \
	F(0x01, 12, /* LD BC, nn */                                                \
			write_reg(BC, read_nn()))                                          \
	F(0x02, 8, /* LD (BC), A */                                                \
			write_reg(BC, read_reg(A)))                                        \
	F(0x03, 8,  /* INC BC */                                                   \
			write_reg(BC, INCR(read_reg(BC))))                                 \
	F(0x04, 4, /* INC B */                                                     \
			mem_t b_orig = read_reg(B);                                        \
			write_reg(B, INCR(b_orig));                                        \
			write_f(fZ, 1, EQUALS(read_reg(B), lift(0)));                      \
			write_f(fN, 0, TRUE);                                              \
			write_f(fH, 1, if_carry(3, b_orig, lift(1))))                      \
	F(0x05, 4,  write_reg(B, DECR(read_reg(B))))              /* DEC B FIXME*/ \
	F(0x06, 8,  write_reg(B, read_n()))                       /* LD B, n */    \
	F(0x07, 8,  perror("Unimplemented."))          /* */                       \
	F(0x08, 8, /* LD (nn), SP */                                               \
			write_mem(read_nn(), read_reg(SP)))          /* */                 \
	F(0x09, 8,  perror("Unimplemented."))          /* */                       \

#define INSTR_FUNCS(OP, CYCLES, CMDS) void exec##OP() \
	{ CMDS; set_lclock(CYCLES); }
#define INSTR_FUNCNAMES(OP, CYCLES, CMDS)  exec##OP,
#define EXEC_INSTR(instr) instructions[instr]()

INSTR_TABLE(INSTR_FUNCS)
void (*instructions[])() = { INSTR_TABLE(INSTR_FUNCNAMES) };

int step_cpu() {
	char* instr = mem.read_byte(cpu->pc);
	EXEC_INSTR((uint8_t)instr);
}
