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
	PC=0, SP=1,

	/* 8 bit registers: */
	A=2, B=3, C=4, D=5, E=6,
	F=7, H=8, L=9,

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
	uint8_t registers[10];
	unsigned int lc;
} cpu_state_t;

cpu_state_t* cpu;
mem_state_t* mem;

/* Place contents of new_val in register r. */
void write_reg(reg_t r, int new_val) {
	switch(r) {
		case AF:
			write_reg(A, new_val & 0xFF);
			write_reg(F, new_val & 0xFF00);
		case BC:
			write_reg(B, new_val & 0xFF);
			write_reg(C, new_val & 0xFF00);
		case DE:
			write_reg(D, new_val & 0xFF);
			write_reg(E, new_val & 0xFF00);
		case HL:
			write_reg(H, new_val & 0xFF);
			write_reg(H, new_val & 0xFF00);
		default:
			cpu->registers[r] = new_val;
	}
}

int read_reg(reg_t r) {
	int result = 0;
	switch(r) {
		case AF:
			result |= read_reg(A);
			result |= (read_reg(F) << 8);
		case BC:
			result |= read_reg(B);
			result |= (read_reg(C) << 8);
		case DE:
			result |= read_reg(D);
			result |= (read_reg(E) << 8);
		case HL:
			result |= read_reg(H);
			result |= (read_reg(L) << 8);
		default:
			return cpu->registers[r];
	}
	return result;
}

int if_carry(int bit, int val1, int val2) {
	return (val1 & (1 << bit)) + (val2 & (1 << bit));
}

int if_borrow(int bit, int val1, int val2) {
	perror("Implement me!");
	return 0;
}

/* FIXME: check; idk if I did this right */
int lrotate(int val, int bound) {
	return (val << 1) | (val & (1 << bound));
}


void write_f(flag_t f, int val, int guard) {
	if (guard) {
		write_reg(F, read_reg(F) | (1 << (4 - f)));
	}
}

int read_nn() {
	perror("Implement me!");
	int bad;
	return bad;
}

int read_n() {
	int result;
	//result.bit8; = mem.read_byte(pc+1);
	return result;
}

void set_lclock(int lclock) {
	cpu->lc = lclock;
}

int cpu_init(mem_state_t* new_mem) {
	mem = new_mem;
	cpu  = malloc(sizeof(cpu_state_t)); /* FIXME: can you do this? */
	write_reg(SP, 0x0000);
	return 0;
}

int cpu_cleanup() {
	free(cpu);
	return 0;
}

void write_mem(int addr, int val) {
	perror("Implement me!");
}

#define TRUE  1
#define FALSE 0

#define INSTR_TABLE(F)                                            \
	F(0x00, /* NOP */                                         \
			asm("nop");                               \
			set_lclock(4))                            \
	F(0x01, /* LD BC, nn */                                   \
			write_reg(BC, read_nn());                 \
			set_lclock(12))                           \
	F(0x02, /* LD (BC), A */                                  \
			write_reg(BC, read_reg(A));               \
			set_lclock(8))                            \
	F(0x03,  /* INC BC */                                     \
			write_reg(BC, read_reg(BC)+1);            \
			set_lclock(8))                            \
	F(0x04, /* INC B */                                       \
			int b_orig = read_reg(B);                 \
			int result = b_orig+1;                    \
			write_reg(B, result);                     \
			write_f(fZ, 1, read_reg(B) == 0);         \
			write_f(fN, 0, TRUE);                     \
			write_f(fH, 1, if_carry(3, b_orig, 1));   \
			set_lclock(8))                            \
	F(0x05, /* DEC B FIXME */                                 \
			int b_orig = read_reg(B);                 \
			int result = b_orig-1;                    \
			write_reg(B, result);                     \
			write_f(fZ, 1, result == 0);              \
			write_f(fN, 1, TRUE);                     \
			write_f(fH, 1, !if_borrow(4, b_orig, 1)); \
			set_lclock(4))                            \
	F(0x06, /* LD B, n */                                     \
			write_reg(B, read_n());                   \
			set_lclock(8))                            \
	F(0x07, /* RLC A */                                       \
			int a_orig = read_reg(A);                 \
			int result = lrotate(a_orig, 7);          \
			write_reg(A, result);                     \
			write_f(fC, a_orig & (1 << 7), TRUE);     \
			write_f(fZ, 1, result == 0);              \
			write_f(fN, 0, TRUE);                     \
			write_f(fH, 0, TRUE))                     \
	F(0x08, /* LD (nn), SP */                                 \
			write_mem(read_nn(), read_reg(SP));       \
			set_lclock(8))                            \
	F(0x09, perror("Unimplemented."))                         \
	F(0x0a, perror("Unimplemented."))                         \
	F(0x0b, perror("Unimplemented."))                         \
	F(0x0c, perror("Unimplemented."))                         \
	F(0x0d, perror("Unimplemented."))                         \
	F(0x0e, perror("Unimplemented."))                         \
	F(0x0f, perror("Unimplemented."))                         \
	F(0x10, perror("Unimplemented."))                         \
	F(0x11, perror("Unimplemented."))                         \
	F(0x12, perror("Unimplemented."))                         \
	F(0x13, perror("Unimplemented."))                         \
	F(0x14, perror("Unimplemented."))                         \
	F(0x15, perror("Unimplemented."))                         \
	F(0x16, perror("Unimplemented."))                         \
	F(0x17, perror("Unimplemented."))                         \
	F(0x18, perror("Unimplemented."))                         \
	F(0x19, perror("Unimplemented."))                         \
	F(0x1a, perror("Unimplemented."))                         \
	F(0x1b, perror("Unimplemented."))                         \
	F(0x1c, perror("Unimplemented."))                         \
	F(0x1d, perror("Unimplemented."))                         \
	F(0x1e, perror("Unimplemented."))                         \
	F(0x1f, perror("Unimplemented."))                         \
	F(0x20, perror("Unimplemented."))                         \
	F(0x21, /* LD HL, nn */                                   \
			write_reg(HL, read_nn());                 \
			set_lclock(12))                           \
	F(0x22, perror("Unimplemented."))                         \
	F(0x23, perror("Unimplemented."))                         \
	F(0x24, perror("Unimplemented."))                         \
	F(0x25, perror("Unimplemented."))                         \
	F(0x26, perror("Unimplemented."))                         \
	F(0x27, perror("Unimplemented."))                         \
	F(0x28, perror("Unimplemented."))                         \
	F(0x29, perror("Unimplemented."))                         \
	F(0x2a, perror("Unimplemented."))                         \
	F(0x2b, perror("Unimplemented."))                         \
	F(0x2c, perror("Unimplemented."))                         \
	F(0x2d, perror("Unimplemented."))                         \
	F(0x2e, perror("Unimplemented."))                         \
	F(0x2f, perror("Unimplemented."))                         \
	F(0x30, perror("Unimplemented."))                         \
	F(0x31, /* LD SP, nn */                                   \
			write_reg(SP, read_nn()))                 \
	F(0x32, perror("Unimplemented."))                         \
	F(0x33, perror("Unimplemented."))                         \
	F(0x34, perror("Unimplemented."))                         \
	F(0x35, perror("Unimplemented."))                         \
	F(0x36, perror("Unimplemented."))                         \
	F(0x37, perror("Unimplemented."))                         \
	F(0x38, perror("Unimplemented."))                         \
	F(0x39, perror("Unimplemented."))                         \
	F(0x3a, perror("Unimplemented."))                         \
	F(0x3b, perror("Unimplemented."))                         \
	F(0x3c, perror("Unimplemented."))                         \
	F(0x3d, perror("Unimplemented."))                         \
	F(0x3e, perror("Unimplemented."))                         \
	F(0x3f, perror("Unimplemented."))                         \
	F(0x40, perror("Unimplemented."))                         \
	F(0x41, perror("Unimplemented."))                         \
	F(0x42, perror("Unimplemented."))                         \
	F(0x43, perror("Unimplemented."))                         \
	F(0x44, perror("Unimplemented."))                         \
	F(0x45, perror("Unimplemented."))                         \
	F(0x46, perror("Unimplemented."))                         \
	F(0x47, perror("Unimplemented."))                         \
	F(0x48, perror("Unimplemented."))                         \
	F(0x49, perror("Unimplemented."))                         \
	F(0x4a, perror("Unimplemented."))                         \
	F(0x4b, perror("Unimplemented."))                         \
	F(0x4c, perror("Unimplemented."))                         \
	F(0x4d, perror("Unimplemented."))                         \
	F(0x4e, perror("Unimplemented."))                         \
	F(0x4f, perror("Unimplemented."))                         \
	F(0x50, perror("Unimplemented."))                         \
	F(0x51, perror("Unimplemented."))                         \
	F(0x52, perror("Unimplemented."))                         \
	F(0x53, perror("Unimplemented."))                         \
	F(0x54, perror("Unimplemented."))                         \
	F(0x55, perror("Unimplemented."))                         \
	F(0x56, perror("Unimplemented."))                         \
	F(0x57, perror("Unimplemented."))                         \
	F(0x58, perror("Unimplemented."))                         \
	F(0x59, perror("Unimplemented."))                         \
	F(0x5a, perror("Unimplemented."))                         \
	F(0x5b, perror("Unimplemented."))                         \
	F(0x5c, perror("Unimplemented."))                         \
	F(0x5d, perror("Unimplemented."))                         \
	F(0x5e, perror("Unimplemented."))                         \
	F(0x5f, perror("Unimplemented."))                         \
	F(0x60, perror("Unimplemented."))                         \
	F(0x61, perror("Unimplemented."))                         \
	F(0x62, perror("Unimplemented."))                         \
	F(0x63, perror("Unimplemented."))                         \
	F(0x64, perror("Unimplemented."))                         \
	F(0x65, perror("Unimplemented."))                         \
	F(0x66, perror("Unimplemented."))                         \
	F(0x67, perror("Unimplemented."))                         \
	F(0x68, perror("Unimplemented."))                         \
	F(0x69, perror("Unimplemented."))                         \
	F(0x6a, perror("Unimplemented."))                         \
	F(0x6b, perror("Unimplemented."))                         \
	F(0x6c, perror("Unimplemented."))                         \
	F(0x6d, perror("Unimplemented."))                         \
	F(0x6e, perror("Unimplemented."))                         \
	F(0x6f, perror("Unimplemented."))                         \
	F(0x70, perror("Unimplemented."))                         \
	F(0x71, perror("Unimplemented."))                         \
	F(0x72, perror("Unimplemented."))                         \
	F(0x73, perror("Unimplemented."))                         \
	F(0x74, perror("Unimplemented."))                         \
	F(0x75, perror("Unimplemented."))                         \
	F(0x76, perror("Unimplemented."))                         \
	F(0x77, perror("Unimplemented."))                         \
	F(0x78, perror("Unimplemented."))                         \
	F(0x79, perror("Unimplemented."))                         \
	F(0x7a, perror("Unimplemented."))                         \
	F(0x7b, perror("Unimplemented."))                         \
	F(0x7c, perror("Unimplemented."))                         \
	F(0x7d, perror("Unimplemented."))                         \
	F(0x7e, perror("Unimplemented."))                         \
	F(0x7f, perror("Unimplemented."))                         \
	F(0x80, perror("Unimplemented."))                         \
	F(0x81, perror("Unimplemented."))                         \
	F(0x82, perror("Unimplemented."))                         \
	F(0x83, perror("Unimplemented."))                         \
	F(0x84, perror("Unimplemented."))                         \
	F(0x85, perror("Unimplemented."))                         \
	F(0x86, perror("Unimplemented."))                         \
	F(0x87, perror("Unimplemented."))                         \
	F(0x88, perror("Unimplemented."))                         \
	F(0x89, perror("Unimplemented."))                         \
	F(0x8a, perror("Unimplemented."))                         \
	F(0x8b, perror("Unimplemented."))                         \
	F(0x8c, perror("Unimplemented."))                         \
	F(0x8d, perror("Unimplemented."))                         \
	F(0x8e, perror("Unimplemented."))                         \
	F(0x8f, perror("Unimplemented."))                         \
	F(0x90, perror("Unimplemented."))                         \
	F(0x91, perror("Unimplemented."))                         \
	F(0x92, perror("Unimplemented."))                         \
	F(0x93, perror("Unimplemented."))                         \
	F(0x94, perror("Unimplemented."))                         \
	F(0x95, perror("Unimplemented."))                         \
	F(0x96, perror("Unimplemented."))                         \
	F(0x97, perror("Unimplemented."))                         \
	F(0x98, perror("Unimplemented."))                         \
	F(0x99, perror("Unimplemented."))                         \
	F(0x9a, perror("Unimplemented."))                         \
	F(0x9b, perror("Unimplemented."))                         \
	F(0x9c, perror("Unimplemented."))                         \
	F(0x9d, perror("Unimplemented."))                         \
	F(0x9e, perror("Unimplemented."))                         \
	F(0x9f, perror("Unimplemented."))                         \
	F(0xa0, perror("Unimplemented."))                         \
	F(0xa1, perror("Unimplemented."))                         \
	F(0xa2, perror("Unimplemented."))                         \
	F(0xa3, perror("Unimplemented."))                         \
	F(0xa4, perror("Unimplemented."))                         \
	F(0xa5, perror("Unimplemented."))                         \
	F(0xa6, perror("Unimplemented."))                         \
	F(0xa7, perror("Unimplemented."))                         \
	F(0xa8, perror("Unimplemented."))                         \
	F(0xa9, perror("Unimplemented."))                         \
	F(0xaa, perror("Unimplemented."))                         \
	F(0xab, perror("Unimplemented."))                         \
	F(0xac, perror("Unimplemented."))                         \
	F(0xad, perror("Unimplemented."))                         \
	F(0xae, perror("Unimplemented."))                         \
	F(0xaf, /* XOR A */                                       \
			int result = read_reg(A) ^ read_reg(A);   \
			write_reg(A, result);                     \
			write_f(fZ, 1, result == 0);              \
			write_f(fN, 0, TRUE);                     \
			write_f(fH, 0, TRUE);                     \
			write_f(fC, 0, TRUE);                     \
			set_lclock(4))                            \
	F(0xb0, perror("Unimplemented."))                         \
	F(0xb1, perror("Unimplemented."))                         \
	F(0xb2, perror("Unimplemented."))                         \
	F(0xb3, perror("Unimplemented."))                         \
	F(0xb4, perror("Unimplemented."))                         \
	F(0xb5, perror("Unimplemented."))                         \
	F(0xb6, perror("Unimplemented."))                         \
	F(0xb7, perror("Unimplemented."))                         \
	F(0xb8, perror("Unimplemented."))                         \
	F(0xb9, perror("Unimplemented."))                         \
	F(0xba, perror("Unimplemented."))                         \
	F(0xbb, perror("Unimplemented."))                         \
	F(0xbc, perror("Unimplemented."))                         \
	F(0xbd, perror("Unimplemented."))                         \
	F(0xbe, perror("Unimplemented."))                         \
	F(0xbf, perror("Unimplemented."))                         \
	F(0xc0, perror("Unimplemented."))                         \
	F(0xc1, perror("Unimplemented."))                         \
	F(0xc2, perror("Unimplemented."))                         \
	F(0xc3, perror("Unimplemented."))                         \
	F(0xc4, perror("Unimplemented."))                         \
	F(0xc5, perror("Unimplemented."))                         \
	F(0xc6, perror("Unimplemented."))                         \
	F(0xc7, perror("Unimplemented."))                         \
	F(0xc8, perror("Unimplemented."))                         \
	F(0xc9, perror("Unimplemented."))                         \
	F(0xca, perror("Unimplemented."))                         \
	F(0xcb, perror("Unimplemented."))                         \
	F(0xcc, perror("Unimplemented."))                         \
	F(0xcd, perror("Unimplemented."))                         \
	F(0xce, perror("Unimplemented."))                         \
	F(0xcf, perror("Unimplemented."))                         \
	F(0xd0, perror("Unimplemented."))                         \
	F(0xd1, perror("Unimplemented."))                         \
	F(0xd2, perror("Unimplemented."))

#define INSTR_FUNCS(OP, CMDS) void exec##OP() { CMDS; }
#define INSTR_FUNCNAMES(OP, CMDS)  exec##OP,
#define EXEC_INSTR(instr) instructions[instr]()

INSTR_TABLE(INSTR_FUNCS)
void (*instructions[])() = { INSTR_TABLE(INSTR_FUNCNAMES) };

#define EXEC_INSTR(instr) instructions[instr]()

void step_cpu() {
	int pc = read_reg(PC);
	char* instr = read_byte(mem, pc);
	EXEC_INSTR((int)instr);
	write_reg(PC, pc+1);
}
