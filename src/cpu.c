#include "stdint.h"
#include "string.h"
#include "../include/mem.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define TRUE  1
#define FALSE 0

#define SIZE_16(val) (val >= 0x0000 && val <= 0xFFFF)
#define SIZE_8(val)  (val >= 0x00 && val <= 0xFF)

#define SET_BIT(num, bit, x) (num | (x << bit))

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
	int registers[10];
	unsigned int lc;
} cpu_state_t;

cpu_state_t* cpu;
mem_state_t* mem;

/* Place contents of new_val in register r. */
void write_reg(reg_t r, int new_val) {
	switch(r) {
		case AF:
			assert(SIZE_16(new_val));
			write_reg(A, new_val & 0xFF);
			write_reg(F, new_val >> 8);
			break;
		case BC:
			assert(SIZE_16(new_val));
			write_reg(B, new_val & 0xFF);
			write_reg(C, new_val >> 8);
			break;
		case DE:
			assert(SIZE_16(new_val));
			write_reg(D, new_val & 0xFF);
			write_reg(E, new_val >> 8);
			break;
		case HL:
			assert(SIZE_16(new_val));
			write_reg(H, new_val & 0xFF);
			write_reg(L, new_val >> 8);
			break;
		case PC: case SP:
			assert(SIZE_16(new_val));
			cpu->registers[r] = new_val;
			break;
		case A: case B: case C: case D: case E:
		case F: case H: case L:
			assert(SIZE_8(new_val));
			cpu->registers[r] = (new_val & 0xFF);
	}
}

#define ADV_PC(count) write_reg(PC, read_reg(PC)+count)

int read_reg(reg_t r) {
	int result = 0;
	switch(r) {
		case AF:
			result |= read_reg(A);
			result |= (read_reg(F) << 8);
			break;
		case BC:
			result |= read_reg(B);
			result |= (read_reg(C) << 8);
			break;
		case DE:
			result |= read_reg(D);
			result |= (read_reg(E) << 8);
			break;
		case HL:
			result |= read_reg(H);
			result |= (read_reg(L) << 8);
			break;
		default:
			return cpu->registers[r];
	}
	return result;
}

int if_carry(int bit, int val1, int val2) {
	return (val1 & (1 << bit)) + (val2 & (1 << bit));
}

int if_borrow4(int val1, int val2) {
	return (val1 & 0xF) - (val2 & 0xF) < 0;
}

/* FIXME: check; idk if I did this right */
int lrotate(int val, int bound) {
	return (val << 1) | (val & (1 << bound));
}

void write_f(flag_t f, int val, int guard) {
	if (guard) {
		int bit    = 4 - f;
		int result = (read_reg(F) & ~(1UL << bit)) | (val << bit);
		write_reg(F, result);
	}
}

int read_f(flag_t f) {
	return (read_reg(F) & (1UL << (4 - f)));
}

int read_nn() {
	int byte1 = read_byte(mem, read_reg(PC)+1);
	int byte2 = read_byte(mem, read_reg(PC)+2);

	int result = 0;
	result |= byte1;
	result |= (byte2 << 8);

	return result;
}

/** Return the byte contained in the address PC+1. */
int read_n() {
	return read_byte(mem, read_reg(PC)+1);
}

void set_lclock(int lclock) {
	cpu->lc = lclock;
}

int init_cpu(mem_state_t* new_mem) {
	mem = new_mem;
	cpu  = malloc(sizeof(cpu_state_t)); /* FIXME: can you do this? */

	/* Zero out 16bit registers */
	write_reg(PC, 0x0000);
	write_reg(SP, 0x0000);

	/* Zero out 8bit registers */
	write_reg(A, 0x00);
	write_reg(B, 0x00);
	write_reg(C, 0x00);
	write_reg(D, 0x00);
	write_reg(E, 0x00);
	write_reg(F, 0x00);
	write_reg(H, 0x00);
	write_reg(L, 0x00);

	return 0;
}

int cpu_cleanup() {
	free(cpu);
	return 0;
}

int push(int val) {
	write_reg(SP, read_reg(SP)-2);
	write_byte(mem, read_reg(SP), val);
	return 0;
}

int pop() {
	int result = read_byte(mem, SP);
	write_reg(SP, read_reg(SP)+2);
	return result;
}

void pp_regs() {
	printf("REGISTERS:\n");
	printf("[PC: %#06x]", read_reg(PC));
	printf("[SP: %#06x]", read_reg(SP));
	printf("[A : %#06x]", read_reg(A));
	printf("[B : %#06x]\n", read_reg(B));
	printf("[C : %#06x]", read_reg(C));
	printf("[D : %#06x]", read_reg(D));
	printf("[E : %#06x]", read_reg(E));
	printf("[F : %#06x]\n", read_reg(F));
	printf("[H : %#06x]", read_reg(H));
	printf("[L : %#06x]", read_reg(L));
	printf("[AF: %#06x]", read_reg(AF));
	printf("[BC: %#06x]\n", read_reg(BC));
	printf("[DE: %#06x]", read_reg(DE));
	printf("[HL: %#06x]\n", read_reg(HL));
}

void pp_flags() {
	printf("FLAGS: ");
	printf("[Z: %d][N: %d][H: %d][C: %d]\n",
			read_f(fZ) ? 1 : 0,
			read_f(fN) ? 1 : 0,
			read_f(fH) ? 1 : 0,
			read_f(fC) ? 1 : 0);
}

/* Holds information about each instruction. */
typedef struct {
	char* iname; /* Instruction name (e.g. 'XOR A') */
	int (*ifunc)(void); /* Internal function pointer */
	int cycles; /* How many cycles execute with this instruction */
} instr_t;

#define CBP_INSTR_TABLE(F)                                            \
	F(0x00, return -1)                                            \
	F(0x01, return -1)                                            \
	F(0x02, return -1)                                            \
	F(0x03, return -1)                                            \
	F(0x04, return -1)                                            \
	F(0x05, return -1)                                            \
	F(0x06, return -1)                                            \
	F(0x07, return -1)                                            \
	F(0x08, return -1)                                            \
	F(0x09, return -1)                                            \
	F(0x0a, return -1)                                            \
	F(0x0b, return -1)                                            \
	F(0x0c, return -1)                                            \
	F(0x0d, return -1)                                            \
	F(0x0e, return -1)                                            \
	F(0x0f, return -1)                                            \
	F(0x10, return -1)                                            \
	F(0x11, /* RL C */                                            \
			write_reg(C, lrotate(read_reg(C), 8));        \
			ADV_PC(1))                                    \
	F(0x12, return -1)                                            \
	F(0x13, return -1)                                            \
	F(0x14, return -1)                                            \
	F(0x15, return -1)                                            \
	F(0x16, return -1)                                            \
	F(0x17, return -1)                                            \
	F(0x18, return -1)                                            \
	F(0x19, return -1)                                            \
	F(0x1a, return -1)                                            \
	F(0x1b, return -1)                                            \
	F(0x1c, return -1)                                            \
	F(0x1d, return -1)                                            \
	F(0x1e, return -1)                                            \
	F(0x1f, return -1)                                            \
	F(0x20, return -1)                                            \
	F(0x21, return -1)                                            \
	F(0x22, return -1)                                            \
	F(0x23, return -1)                                            \
	F(0x24, return -1)                                            \
	F(0x25, return -1)                                            \
	F(0x26, return -1)                                            \
	F(0x27, return -1)                                            \
	F(0x28, return -1)                                            \
	F(0x29, return -1)                                            \
	F(0x2a, return -1)                                            \
	F(0x2b, return -1)                                            \
	F(0x2c, return -1)                                            \
	F(0x2d, return -1)                                            \
	F(0x2e, return -1)                                            \
	F(0x2f, return -1)                                            \
	F(0x30, return -1)                                            \
	F(0x31, return -1)                                            \
	F(0x32, return -1)                                            \
	F(0x33, return -1)                                            \
	F(0x34, return -1)                                            \
	F(0x35, return -1)                                            \
	F(0x36, return -1)                                            \
	F(0x37, return -1)                                            \
	F(0x38, return -1)                                            \
	F(0x39, return -1)                                            \
	F(0x3a, return -1)                                            \
	F(0x3b, return -1)                                            \
	F(0x3c, return -1)                                            \
	F(0x3d, return -1)                                            \
	F(0x3e, return -1)                                            \
	F(0x3f, return -1)                                            \
	F(0x40, return -1)                                            \
	F(0x41, return -1)                                            \
	F(0x42, return -1)                                            \
	F(0x43, return -1)                                            \
	F(0x44, return -1)                                            \
	F(0x45, return -1)                                            \
	F(0x46, return -1)                                            \
	F(0x47, return -1)                                            \
	F(0x48, return -1)                                            \
	F(0x49, return -1)                                            \
	F(0x4a, return -1)                                            \
	F(0x4b, return -1)                                            \
	F(0x4c, return -1)                                            \
	F(0x4d, return -1)                                            \
	F(0x4e, return -1)                                            \
	F(0x4f, return -1)                                            \
	F(0x50, return -1)                                            \
	F(0x51, return -1)                                            \
	F(0x52, return -1)                                            \
	F(0x53, return -1)                                            \
	F(0x54, return -1)                                            \
	F(0x55, return -1)                                            \
	F(0x56, return -1)                                            \
	F(0x57, return -1)                                            \
	F(0x58, return -1)                                            \
	F(0x59, return -1)                                            \
	F(0x5a, return -1)                                            \
	F(0x5b, return -1)                                            \
	F(0x5c, return -1)                                            \
	F(0x5d, return -1)                                            \
	F(0x5e, return -1)                                            \
	F(0x5f, return -1)                                            \
	F(0x60, return -1)                                            \
	F(0x61, return -1)                                            \
	F(0x62, return -1)                                            \
	F(0x63, return -1)                                            \
	F(0x64, return -1)                                            \
	F(0x65, return -1)                                            \
	F(0x66, return -1)                                            \
	F(0x67, return -1)                                            \
	F(0x68, return -1)                                            \
	F(0x69, return -1)                                            \
	F(0x6a, return -1)                                            \
	F(0x6b, return -1)                                            \
	F(0x6c, return -1)                                            \
	F(0x6d, return -1)                                            \
	F(0x6e, return -1)                                            \
	F(0x6f, return -1)                                            \
	F(0x70, return -1)                                            \
	F(0x71, return -1)                                            \
	F(0x72, return -1)                                            \
	F(0x73, return -1)                                            \
	F(0x74, return -1)                                            \
	F(0x75, return -1)                                            \
	F(0x76, return -1)                                            \
	F(0x77, return -1)                                            \
	F(0x78, return -1)                                            \
	F(0x79, return -1)                                            \
	F(0x7a, return -1)                                            \
	F(0x7b, return -1)                                            \
	F(0x7c, /* BIT 7, H */                                        \
			printf("y this not execute                    \ n"); \
			write_f(fZ, 1, 1 & ((1 << 7) & read_reg(H))); \
			write_f(fN, 0, TRUE);                         \
			write_f(fH, 1, TRUE);                         \
			write_reg(PC, read_reg(PC)+2))                \
	F(0x7d, return -1)                                            \
	F(0x7e, return -1)                                            \
	F(0x7f, return -1)                                            \
	F(0x80, return -1)                                            \
	F(0x81, return -1)                                            \
	F(0x82, return -1)                                            \
	F(0x83, return -1)                                            \
	F(0x84, return -1)                                            \
	F(0x85, return -1)                                            \
	F(0x86, return -1)                                            \
	F(0x87, return -1)                                            \
	F(0x88, return -1)                                            \
	F(0x89, return -1)                                            \
	F(0x8a, return -1)                                            \
	F(0x8b, return -1)                                            \
	F(0x8c, return -1)                                            \
	F(0x8d, return -1)                                            \
	F(0x8e, return -1)                                            \
	F(0x8f, return -1)                                            \
	F(0x90, return -1)                                            \
	F(0x91, return -1)                                            \
	F(0x92, return -1)                                            \
	F(0x93, return -1)                                            \
	F(0x94, return -1)                                            \
	F(0x95, return -1)                                            \
	F(0x96, return -1)                                            \
	F(0x97, return -1)                                            \
	F(0x98, return -1)                                            \
	F(0x99, return -1)                                            \
	F(0x9a, return -1)                                            \
	F(0x9b, return -1)                                            \
	F(0x9c, return -1)                                            \
	F(0x9d, return -1)                                            \
	F(0x9e, return -1)                                            \
	F(0x9f, return -1)                                            \
	F(0xa0, return -1)                                            \
	F(0xa1, return -1)                                            \
	F(0xa2, return -1)                                            \
	F(0xa3, return -1)                                            \
	F(0xa4, return -1)                                            \
	F(0xa5, return -1)                                            \
	F(0xa6, return -1)                                            \
	F(0xa7, return -1)                                            \
	F(0xa8, return -1)                                            \
	F(0xa9, return -1)                                            \
	F(0xaa, return -1)                                            \
	F(0xab, return -1)                                            \
	F(0xac, return -1)                                            \
	F(0xad, return -1)                                            \
	F(0xae, return -1)                                            \
	F(0xaf, return -1)                                            \
	F(0xb0, return -1)                                            \
	F(0xb1, return -1)                                            \
	F(0xb2, return -1)                                            \
	F(0xb3, return -1)                                            \
	F(0xb4, return -1)                                            \
	F(0xb5, return -1)                                            \
	F(0xb6, return -1)                                            \
	F(0xb7, return -1)                                            \
	F(0xb8, return -1)                                            \
	F(0xb9, return -1)                                            \
	F(0xba, return -1)                                            \
	F(0xbb, return -1)                                            \
	F(0xbc, return -1)                                            \
	F(0xbd, return -1)                                            \
	F(0xbe, return -1)                                            \
	F(0xbf, return -1)                                            \
	F(0xc0, return -1)                                            \
	F(0xc1, return -1)                                            \
	F(0xc2, return -1)                                            \
	F(0xc3, return -1)                                            \
	F(0xc4, return -1)                                            \
	F(0xc5, return -1)                                            \
	F(0xc6, return -1)                                            \
	F(0xc7, return -1)                                            \
	F(0xc8, return -1)                                            \
	F(0xc9, return -1)                                            \
	F(0xca, return -1)                                            \
	F(0xcb, return -1)                                            \
	F(0xcc, return -1)                                            \
	F(0xcd, return -1)                                            \
	F(0xce, return -1)                                            \
	F(0xcf, return -1)                                            \
	F(0xd0, return -1)                                            \
	F(0xd1, return -1)                                            \
	F(0xd2, return -1)                                            \
	F(0xd3, return -1)                                            \
	F(0xd4, return -1)                                            \
	F(0xd5, return -1)                                            \
	F(0xd6, return -1)                                            \
	F(0xd7, return -1)                                            \
	F(0xd8, return -1)                                            \
	F(0xd9, return -1)                                            \
	F(0xda, return -1)                                            \
	F(0xdb, return -1)                                            \
	F(0xdc, return -1)                                            \
	F(0xdd, return -1)                                            \
	F(0xde, return -1)                                            \
	F(0xdf, return -1)                                            \
	F(0xe0, return -1)                                            \
	F(0xe1, return -1)                                            \
	F(0xe2, return -1)                                            \
	F(0xe3, return -1)                                            \
	F(0xe4, return -1)                                            \
	F(0xe5, return -1)                                            \
	F(0xe6, return -1)                                            \
	F(0xe7, return -1)                                            \
	F(0xe8, return -1)                                            \
	F(0xe9, return -1)                                            \
	F(0xea, return -1)                                            \
	F(0xeb, return -1)                                            \
	F(0xec, return -1)                                            \
	F(0xed, return -1)                                            \
	F(0xee, return -1)                                            \
	F(0xef, return -1)                                            \
	F(0xf0, return -1)                                            \
	F(0xf1, return -1)                                            \
	F(0xf2, return -1)                                            \
	F(0xf3, return -1)                                            \
	F(0xf4, return -1)                                            \
	F(0xf5, return -1)                                            \
	F(0xf6, return -1)                                            \
	F(0xf7, return -1)                                            \
	F(0xf8, return -1)                                            \
	F(0xf9, return -1)                                            \
	F(0xfa, return -1)                                            \
	F(0xfb, return -1)                                            \
	F(0xfc, return -1)                                            \
	F(0xfd, return -1)                                            \
	F(0xfe, return -1)                                            \
	F(0xff, return -1)

#define CBP_INSTR_FUNCS(OP, CMDS) int exec_cbp##OP() { CMDS; return 0; }
#define CBP_INSTR_FUNCNAMES(OP, CMDS) exec_cbp##OP,
#define EXEC_CBP_INSTR(instr) cbp_instrs[instr]()

CBP_INSTR_TABLE(CBP_INSTR_FUNCS)
int (*cbp_instrs[])() = { CBP_INSTR_TABLE(CBP_INSTR_FUNCNAMES) };

#define EXEC_CBP_INSTR(instr)  cbp_instrs[instr]()

#define BASE_INSTR_TABLE(F)                                                 \
	F(0x00, 4, "NOP",                                                   \
			asm("nop");                                         \
			ADV_PC(1))                                          \
	F(0x01, 12, "LD BC, nn",                                            \
			write_reg(BC, read_nn());                           \
			ADV_PC(3))                                          \
	F(0x02, 8, "LD (BC), A",                                            \
			write_reg(BC, read_reg(A));                         \
			ADV_PC(1))                                          \
	F(0x03, 8, "INC BC",                                                \
			write_reg(BC, read_reg(BC)+1);                      \
			ADV_PC(1))                                          \
	F(0x04, 8, "INC B",                                                 \
			int b_orig = read_reg(B);                           \
			int result = b_orig+1;                              \
			write_reg(B, result);                               \
			write_f(fZ, 1, read_reg(B) == 0);                   \
			write_f(fN, 0, TRUE);                               \
			write_f(fH, 1, if_carry(3, b_orig, 1));             \
			ADV_PC(1))                                          \
	F(0x05, 4, "DEC B FIXME",                                           \
			int b_orig = read_reg(B);                           \
			int result = b_orig-1;                              \
			write_reg(B, result);                               \
			write_f(fZ, 1, result == 0);                        \
			write_f(fN, 1, TRUE);                               \
			write_f(fH, 1, !if_borrow4(b_orig, 1));             \
			ADV_PC(1))                                          \
	F(0x06, 8, "LD B, n",                                               \
			write_reg(B, read_n());                             \
			ADV_PC(2))                                          \
	F(0x07, 4, "RLC A",                                                 \
			int a_orig = read_reg(A);                           \
			int result = lrotate(a_orig, 7);                    \
			write_reg(A, result);                               \
			write_f(fC, a_orig & (1 << 7), TRUE);               \
			write_f(fZ, 1, result == 0);                        \
			write_f(fN, 0, TRUE);                               \
			write_f(fH, 0, TRUE);                               \
			ADV_PC(1))                                          \
	F(0x08, 8, "LD (nn), SP",                                           \
			write_byte(mem, read_nn(), read_reg(SP));           \
			ADV_PC(3))                                          \
	F(0x09, 0, "", return -1)                                           \
	F(0x0a, 0, "", return -1)                                           \
	F(0x0b, 0, "", return -1)                                           \
	F(0x0c, 4, "INC C",                                                 \
			int result = read_reg(C)+1;                         \
			write_f(fZ, 1, result == 0);                        \
			write_f(fN, 0, TRUE);                               \
			write_f(fH, 1, if_carry(3, read_reg(C), 1));        \
			write_reg(C, result);                               \
			ADV_PC(1))                                          \
	F(0x0d, 0, "", return -1)                                           \
	F(0x0e, 8, "LD C, n",                                               \
			write_reg(C, read_n());                             \
			ADV_PC(2))                                          \
	F(0x0f, 0, "", return -1)                                           \
	F(0x10, 0, "", return -1)                                           \
	F(0x11, 12, "LD DE, nn",                                            \
			write_reg(DE, read_nn());                           \
			ADV_PC(3))                                          \
	F(0x12, 0, "", return -1)                                           \
	F(0x13, 8, "INC DE",                                                \
			write_reg(DE, read_reg(DE)+1);                      \
			ADV_PC(1))                                          \
	F(0x14, 0, "", return -1)                                           \
	F(0x15, 0, "", return -1)                                           \
	F(0x16, 0, "", return -1)                                           \
	F(0x17, 0, "", return -1)                                           \
	F(0x18, 0, "", return -1)                                           \
	F(0x19, 0, "", return -1)                                           \
	F(0x1a, 8, "LD A, (DE)",                                            \
			write_reg(A, read_byte(mem, read_reg(DE)));         \
			ADV_PC(1))                                          \
	F(0x1b, 0, "", return -1)                                           \
	F(0x1c, 0, "", return -1)                                           \
	F(0x1d, 0, "", return -1)                                           \
	F(0x1e, 0, "", return -1)                                           \
	F(0x1f, 0, "", return -1)                                           \
	F(0x20, 8, "JR NZ, n",                                              \
			int n_addr = PC + read_n();                         \
			if (read_f(fZ) == 0) write_reg(PC, n_addr);         \
			ADV_PC(1))                                          \
	F(0x21, 12, "LD HL, nn",                                            \
			write_reg(HL, read_nn());                           \
			ADV_PC(3))                                          \
	F(0x22, 8, "LD (HL+), A",                                           \
			write_byte(mem, read_reg(HL), read_reg(A)+1);       \
			ADV_PC(1))                                          \
	F(0x23, 8, "INC HL",                                                \
			write_reg(HL, read_reg(HL)+1);                      \
			ADV_PC(1))                                          \
	F(0x24, 0, "", return -1)                                           \
	F(0x25, 0, "", return -1)                                           \
	F(0x26, 0, "", return -1)                                           \
	F(0x27, 0, "", return -1)                                           \
	F(0x28, 0, "", return -1)                                           \
	F(0x29, 0, "", return -1)                                           \
	F(0x2a, 0, "", return -1)                                           \
	F(0x2b, 0, "", return -1)                                           \
	F(0x2c, 0, "", return -1)                                           \
	F(0x2d, 0, "", return -1)                                           \
	F(0x2e, 0, "", return -1)                                           \
	F(0x2f, 0, "", return -1)                                           \
	F(0x30, 0, "", return -1)                                           \
	F(0x31, 12, "LD SP, nn",                                            \
			write_reg(SP, read_nn());                           \
			ADV_PC(3))                                          \
	F(0x32, 8, "LD (HL), A",                                            \
			write_byte(mem, read_reg(HL), read_reg(A)-1);       \
			ADV_PC(1))                                          \
	F(0x33, 0, "", return -1)                                           \
	F(0x34, 12, "INC (HL)",                                             \
			int result = read_byte(mem, read_reg(HL))+1;        \
			write_f(fZ, 1, result == 0);                        \
			write_f(fN, 0, TRUE);                               \
			write_f(fH, 1, if_carry(1, 1, 1));                  \
			write_byte(mem, read_reg(HL), result);              \
			ADV_PC(1))                                          \
	F(0x35, 0, "", return -1)                                           \
	F(0x36, 0, "", return -1)                                           \
	F(0x37, 0, "", return -1)                                           \
	F(0x38, 0, "", return -1)                                           \
	F(0x39, 0, "", return -1)                                           \
	F(0x3a, 0, "", return -1)                                           \
	F(0x3b, 0, "", return -1)                                           \
	F(0x3c, 0, "", return -1)                                           \
	F(0x3d, 0, "", return -1)                                           \
	F(0x3e, 8, "LD A, n",                                               \
			write_reg(A, read_n());                             \
			ADV_PC(2))                                          \
	F(0x3f, 0, "", return -1)                                           \
	F(0x40, 0, "", return -1)                                           \
	F(0x41, 0, "", return -1)                                           \
	F(0x42, 0, "", return -1)                                           \
	F(0x43, 0, "", return -1)                                           \
	F(0x44, 0, "", return -1)                                           \
	F(0x45, 0, "", return -1)                                           \
	F(0x46, 0, "", return -1)                                           \
	F(0x47, 0, "", return -1)                                           \
	F(0x48, 0, "", return -1)                                           \
	F(0x49, 0, "", return -1)                                           \
	F(0x4a, 0, "", return -1)                                           \
	F(0x4b, 0, "", return -1)                                           \
	F(0x4c, 0, "", return -1)                                           \
	F(0x4d, 0, "", return -1)                                           \
	F(0x4e, 0, "", return -1)                                           \
	F(0x4f, 4, "LD C, A",                                               \
			write_reg(C, read_reg(A));                          \
			ADV_PC(1))                                          \
	F(0x50, 0, "", return -1)                                           \
	F(0x51, 0, "", return -1)                                           \
	F(0x52, 0, "", return -1)                                           \
	F(0x53, 0, "", return -1)                                           \
	F(0x54, 0, "", return -1)                                           \
	F(0x55, 0, "", return -1)                                           \
	F(0x56, 0, "", return -1)                                           \
	F(0x57, 0, "", return -1)                                           \
	F(0x58, 0, "", return -1)                                           \
	F(0x59, 0, "", return -1)                                           \
	F(0x5a, 0, "", return -1)                                           \
	F(0x5b, 0, "", return -1)                                           \
	F(0x5c, 0, "", return -1)                                           \
	F(0x5d, 0, "", return -1)                                           \
	F(0x5e, 0, "", return -1)                                           \
	F(0x5f, 0, "", return -1)                                           \
	F(0x60, 0, "", return -1)                                           \
	F(0x61, 0, "", return -1)                                           \
	F(0x62, 0, "", return -1)                                           \
	F(0x63, 0, "", return -1)                                           \
	F(0x64, 0, "", return -1)                                           \
	F(0x65, 0, "", return -1)                                           \
	F(0x66, 0, "", return -1)                                           \
	F(0x67, 0, "", return -1)                                           \
	F(0x68, 0, "", return -1)                                           \
	F(0x69, 0, "", return -1)                                           \
	F(0x6a, 0, "", return -1)                                           \
	F(0x6b, 4, "LD L, E",                                               \
			write_reg(L, read_reg(E));                          \
			ADV_PC(1))                                          \
	F(0x6c, 0, "", return -1)                                           \
	F(0x6d, 0, "", return -1)                                           \
	F(0x6e, 0, "", return -1)                                           \
	F(0x6f, 0, "", return -1)                                           \
	F(0x70, 0, "", return -1)                                           \
	F(0x71, 0, "", return -1)                                           \
	F(0x72, 0, "", return -1)                                           \
	F(0x73, 0, "", return -1)                                           \
	F(0x74, 0, "", return -1)                                           \
	F(0x75, 0, "", return -1)                                           \
	F(0x76, 0, "", return -1)                                           \
	F(0x77, 8, "LD (HL), A",                                            \
			write_byte(mem, read_reg(HL), read_reg(A));         \
			ADV_PC(1))                                          \
	F(0x78, 0, "", return -1)                                           \
	F(0x79, 0, "", return -1)                                           \
	F(0x7a, 0, "", return -1)                                           \
	F(0x7b, 0, "", return -1)                                           \
	F(0x7c, 4, "LD A, H",                                               \
			write_reg(A, read_reg(H));                          \
			ADV_PC(1))                                          \
	F(0x7d, 4, "LD A, L",                                               \
			write_reg(A, read_reg(L));                          \
			ADV_PC(1))                                          \
	F(0x7e, 0, "", return -1)                                           \
	F(0x7f, 0, "", return -1)                                           \
	F(0x80, 0, "", return -1)                                           \
	F(0x81, 0, "", return -1)                                           \
	F(0x82, 0, "", return -1)                                           \
	F(0x83, 0, "", return -1)                                           \
	F(0x84, 0, "", return -1)                                           \
	F(0x85, 0, "", return -1)                                           \
	F(0x86, 0, "", return -1)                                           \
	F(0x87, 0, "", return -1)                                           \
	F(0x88, 0, "", return -1)                                           \
	F(0x89, 0, "", return -1)                                           \
	F(0x8a, 0, "", return -1)                                           \
	F(0x8b, 0, "", return -1)                                           \
	F(0x8c, 0, "", return -1)                                           \
	F(0x8d, 0, "", return -1)                                           \
	F(0x8e, 0, "", return -1)                                           \
	F(0x8f, 0, "", return -1)                                           \
	F(0x90, 0, "", return -1)                                           \
	F(0x91, 0, "", return -1)                                           \
	F(0x92, 0, "", return -1)                                           \
	F(0x93, 0, "", return -1)                                           \
	F(0x94, 0, "", return -1)                                           \
	F(0x95, 0, "", return -1)                                           \
	F(0x96, 0, "", return -1)                                           \
	F(0x97, 0, "", return -1)                                           \
	F(0x98, 0, "", return -1)                                           \
	F(0x99, 0, "", return -1)                                           \
	F(0x9a, 0, "", return -1)                                           \
	F(0x9b, 0, "", return -1)                                           \
	F(0x9c, 0, "", return -1)                                           \
	F(0x9d, 0, "", return -1)                                           \
	F(0x9e, 0, "", return -1)                                           \
	F(0x9f, 4, "SBC A. n",                                              \
			int ncsum = read_reg(A) + read_f(fC);               \
			write_reg(A, read_reg(A) - ncsum);                  \
			ADV_PC(1))                                          \
	F(0xa0, 0, "", return -1)                                           \
	F(0xa1, 0, "", return -1)                                           \
	F(0xa2, 0, "", return -1)                                           \
	F(0xa3, 0, "", return -1)                                           \
	F(0xa4, 0, "", return -1)                                           \
	F(0xa5, 0, "", return -1)                                           \
	F(0xa6, 0, "", return -1)                                           \
	F(0xa7, 0, "", return -1)                                           \
	F(0xa8, 0, "", return -1)                                           \
	F(0xa9, 0, "", return -1)                                           \
	F(0xaa, 0, "", return -1)                                           \
	F(0xab, 0, "", return -1)                                           \
	F(0xac, 0, "", return -1)                                           \
	F(0xad, 0, "", return -1)                                           \
	F(0xae, 0, "", return -1)                                           \
	F(0xaf, 4, "XOR A",                                                 \
			int result = read_reg(A) ^ read_reg(A);             \
			write_reg(A, result);                               \
			write_f(fZ, 1, result == 0);                        \
			write_f(fN, 0, TRUE);                               \
			write_f(fH, 0, TRUE);                               \
			write_f(fC, 0, TRUE);                               \
			ADV_PC(1))                                          \
	F(0xb0, 0, "", return -1)                                           \
	F(0xb1, 0, "", return -1)                                           \
	F(0xb2, 0, "", return -1)                                           \
	F(0xb3, 0, "", return -1)                                           \
	F(0xb4, 0, "", return -1)                                           \
	F(0xb5, 0, "", return -1)                                           \
	F(0xb6, 0, "", return -1)                                           \
	F(0xb7, 0, "", return -1)                                           \
	F(0xb8, 0, "", return -1)                                           \
	F(0xb9, 0, "", return -1)                                           \
	F(0xba, 0, "", return -1)                                           \
	F(0xbb, 0, "", return -1)                                           \
	F(0xbc, 0, "", return -1)                                           \
	F(0xbd, 0, "", return -1)                                           \
	F(0xbe, 8, "CP (HL)",                                               \
			int orig   = read_byte(mem, read_reg(HL));          \
			int result = orig - read_n();                       \
			write_f(fZ, 1, result == 0);                        \
			write_f(fN, 1, TRUE);                               \
			write_f(fH, 1, !if_borrow4(orig, read_n()));        \
	 		write_f(fC, 1, result < 0);                         \
			ADV_PC(2))                                          \
	F(0xbf, 0, "", return -1)                                           \
	F(0xc0, 0, "", return -1)                                           \
	F(0xc1, 0, "", return -1)                                           \
	F(0xc2, 0, "", return -1)                                           \
	F(0xc3, 0, "", return -1)                                           \
	F(0xc4, 0, "", return -1)                                           \
	F(0xc5, 16, "PUSH BC",                                              \
			push(read_reg(BC));                                 \
			ADV_PC(1))                                          \
	F(0xc6, 0, "", return -1)                                           \
	F(0xc7, 0, "", return -1)                                           \
	F(0xc8, 0, "", return -1)                                           \
	F(0xc9, 8, "RET",                                                   \
			write_reg(PC, pop()))                               \
	F(0xca, 0, "", return -1)                                           \
	F(0xcb, 4, "CB",                                                    \
			return EXEC_CBP_INSTR(read_n()))                    \
	F(0xcc, 0, "", return -1)                                           \
	F(0xcd, 12, "CALL nn",                                              \
			push(read_reg(PC)+3);                               \
			write_reg(PC, read_nn()))                           \
	F(0xce, 0, "", return -1)                                           \
	F(0xcf, 0, "", return -1)                                           \
	F(0xd0, 0, "", return -1)                                           \
	F(0xd1, 0, "", return -1)                                           \
	F(0xd2, 0, "", return -1)                                           \
	F(0xd3, 0, "", return -1)                                           \
	F(0xd4, 0, "", return -1)                                           \
	F(0xd5, 0, "", return -1)                                           \
	F(0xd6, 0, "", return -1)                                           \
	F(0xd7, 0, "", return -1)                                           \
	F(0xd8, 0, "", return -1)                                           \
	F(0xd9, 0, "", return -1)                                           \
	F(0xda, 0, "", return -1)                                           \
	F(0xdb, 0, "", return -1)                                           \
	F(0xdc, 0, "", return -1)                                           \
	F(0xdd, 0, "", return -1)                                           \
	F(0xde, 0, "", return -1)                                           \
	F(0xdf, 0, "", return -1)                                           \
	F(0xe0, 12, "LDH (n), A",                                           \
			write_byte(mem, 0xFF00+read_n(), read_reg(A));      \
			ADV_PC(2))                                          \
	F(0xe1, 0, "", return -1)                                           \
	F(0xe2, 8, "LD (C), A",                                             \
			write_byte(mem, 0xFF00 + read_reg(C), read_reg(A)); \
			ADV_PC(1))                                          \
	F(0xe3, 0, "", return -1)                                           \
	F(0xe4, 0, "", return -1)                                           \
	F(0xe5, 16, "PUSH HL",                                              \
			push(read_reg(HL));                                 \
			ADV_PC(1))                                          \
	F(0xe6, 0, "", return -1)                                           \
	F(0xe7, 0, "", return -1)                                           \
	F(0xe8, 0, "", return -1)                                           \
	F(0xe9, 0, "", return -1)                                           \
	F(0xea, 0, "", return -1)                                           \
	F(0xeb, 0, "", return -1)                                           \
	F(0xec, 0, "", return -1)                                           \
	F(0xed, 0, "", return -1)                                           \
	F(0xee, 0, "", return -1)                                           \
	F(0xef, 0, "", return -1)                                           \
	F(0xf0, 0, "", return -1)                                           \
	F(0xf1, 0, "", return -1)                                           \
	F(0xf2, 0, "", return -1)                                           \
	F(0xf3, 0, "", return -1)                                           \
	F(0xf4, 0, "", return -1)                                           \
	F(0xf5, 16, "PUSH AF", push(read_reg(AF)); ADV_PC(1))               \
	F(0xf6, 0, "", return -1)                                           \
	F(0xf7, 0, "", return -1)                                           \
	F(0xf8, 0, "", return -1)                                           \
	F(0xf9, 0, "", return -1)                                           \
	F(0xfa, 0, "", return -1)                                           \
	F(0xfb, 4, "EI",                                                    \
			printf("Interrupts enabled. TODO: interrupts.");    \
			ADV_PC(1))                                          \
	F(0xfc, 0, "", return -1)                                           \
	F(0xfd, 0, "", return -1)                                           \
	F(0xfe, 8, "CP d8",                                                 \
			int result = read_reg(A) - read_n();                \
			write_f(fZ, 1, result == 0);                        \
			write_f(fN, 1, TRUE);                               \
			write_f(fH, 1, !if_borrow4(read_reg(A), read_n())); \
	 		write_f(fC, 1, result < 0);                         \
			ADV_PC(2))                                          \
	F(0xff, 16, "RST 38",                                               \
			push(read_reg(PC));                                 \
			write_reg(PC, read_n());                            \
			ADV_PC(1))

#define BASE_INSTR_STRUCTS(OP, CYCLES, INAME, CMDS) \
	{ .iname=INAME, .ifunc=exec_base##OP, .cycles=CYCLES },
#define BASE_INSTR_FUNCS(OP, CYCLES, INAME, CMDS) \
	int exec_base##OP() { CMDS; set_lclock(CYCLES); return 0; }

BASE_INSTR_TABLE(BASE_INSTR_FUNCS)
instr_t base_instrs[] = { BASE_INSTR_TABLE(BASE_INSTR_STRUCTS) };

#define EXEC_BASE_INSTR(instr) base_instrs[instr].ifunc()
#define PP_INSTR(instr) base_instrs[instr].iname

int step_cpu() {
	int pc = read_reg(PC);
	int instr = read_byte(mem, pc);
#ifdef DEBUG
	printf("-----------------------\n");
	printf("INSTR: \"%s\" [%#x]\n", PP_INSTR(instr), instr, pc);
#endif

	if (EXEC_BASE_INSTR(instr)) {
		printf("Error: unimplemented instruction %#x.\n", instr);
		return -1;
	};
#ifdef DEBUG
	pp_regs();
	pp_flags();
	printf("-----------------------\n");
#endif
	return 0;
}
