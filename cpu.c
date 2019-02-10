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

int read_f(flag_t f) {
	return (read_reg(F) | (1 << (4 - f)));
}

int read_nn() {
	int byte1 = read_byte(mem, read_reg(PC)+1);
	int byte2 = read_byte(mem, read_reg(PC)+2);

	int result = 0;
	result |= byte1;
	result |= (byte2 << 8);

	return result;
}

int read_n() {
	return read_byte(mem, read_reg(PC)+1);
}

void set_lclock(int lclock) {
	cpu->lc = lclock;
}

int init_cpu(mem_state_t* new_mem) {
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

int push(int val) {
	write_reg(SP, read_reg(SP)-2);
	write_mem(read_reg(SP), val);
	return 0;
}

int pop() {
	int result = read_byte(mem, SP);
	write_reg(SP, read_reg(SP)+2);
	return result;
}

void pp_regs() {
	printf("-- Register Status --\n");
	printf("PC: %#x\n", read_reg(PC));
	printf("SP: %#x\n", read_reg(SP));
	printf("A : %#x\n", read_reg(A));
	printf("B : %#x\n", read_reg(B));
	printf("C : %#x\n", read_reg(C));
	printf("D : %#x\n", read_reg(D));
	printf("E : %#x\n", read_reg(E));
	printf("F : %#x\n", read_reg(F));
	printf("H : %#x\n", read_reg(H));
	printf("L : %#x\n", read_reg(L));
	printf("AF: %#x\n", read_reg(AF));
	printf("BC: %#x\n", read_reg(BC));
	printf("DE: %#x\n", read_reg(DE));
	printf("HL: %#x\n", read_reg(HL));
}

typedef struct {
	char* iname;
	int (*ifunc)(void);
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
	F(0x11, return -1)                                            \
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
			write_f(fZ, 1, 1 & ((1 << 7) | read_reg(H))); \
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

#define BASE_INSTR_TABLE(F)                                              \
	F(0x00, "NOP",                                                   \
			asm("nop");                                      \
			set_lclock(4);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0x01, "LD BC, nn",                                             \
			write_reg(BC, read_nn());                        \
			set_lclock(12);                                  \
			write_reg(PC, read_reg(PC)+3))                   \
	F(0x02, "LD (BC), A",                                            \
			write_reg(BC, read_reg(A));                      \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0x03, "INC BC",                                                \
			write_reg(BC, read_reg(BC)+1);                   \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0x04, "INC B",                                                 \
			int b_orig = read_reg(B);                        \
			int result = b_orig+1;                           \
			write_reg(B, result);                            \
			write_f(fZ, 1, read_reg(B) == 0);                \
			write_f(fN, 0, TRUE);                            \
			write_f(fH, 1, if_carry(3, b_orig, 1));          \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0x05, "DEC B FIXME",                                           \
			int b_orig = read_reg(B);                        \
			int result = b_orig-1;                           \
			write_reg(B, result);                            \
			write_f(fZ, 1, result == 0);                     \
			write_f(fN, 1, TRUE);                            \
			write_f(fH, 1, !if_borrow(4, b_orig, 1));        \
			set_lclock(4);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0x06, "LD B, n",                                               \
			write_reg(B, read_n());                          \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+2))                   \
	F(0x07, "RLC A",                                                 \
			int a_orig = read_reg(A);                        \
			int result = lrotate(a_orig, 7);                 \
			write_reg(A, result);                            \
			write_f(fC, a_orig & (1 << 7), TRUE);            \
			write_f(fZ, 1, result == 0);                     \
			write_f(fN, 0, TRUE);                            \
			write_f(fH, 0, TRUE);                            \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0x08, "LD (nn), SP",                                           \
			write_mem(read_nn(), read_reg(SP));              \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+3))                   \
	F(0x09, "", return -1)                                           \
	F(0x0a, "", return -1)                                           \
	F(0x0b, "", return -1)                                           \
	F(0x0c, "INC C",                                                 \
			int result = read_reg(C)+1;                      \
			write_f(fZ, 1, result == 0);                     \
			write_f(fN, 0, TRUE);                            \
			write_f(fH, 1, if_carry(3, read_reg(C), 1));     \
			write_reg(PC, read_reg(PC)+1);                   \
			set_lclock(4);                                   \
			write_reg(C, result))                            \
	F(0x0d, "", return -1)                                           \
	F(0x0e, "LD C, n",                                               \
			write_reg(C, read_n());                          \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+2))                   \
	F(0x0f, "", return -1)                                           \
	F(0x10, "", return -1)                                           \
	F(0x11, "", return -1)                                           \
	F(0x12, "", return -1)                                           \
	F(0x13, "", return -1)                                           \
	F(0x14, "", return -1)                                           \
	F(0x15, "", return -1)                                           \
	F(0x16, "", return -1)                                           \
	F(0x17, "", return -1)                                           \
	F(0x18, "", return -1)                                           \
	F(0x19, "", return -1)                                           \
	F(0x1a, "", return -1)                                           \
	F(0x1b, "", return -1)                                           \
	F(0x1c, "", return -1)                                           \
	F(0x1d, "", return -1)                                           \
	F(0x1e, "", return -1)                                           \
	F(0x1f, "", return -1)                                           \
	F(0x20, "JR NZ, n",                                              \
			int n_addr = PC + read_n();                      \
			if (read_f(fZ) == 0) write_reg(PC, n_addr);      \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0x21, "LD HL, nn",                                             \
			write_reg(HL, read_nn());                        \
			set_lclock(12);                                  \
			write_reg(PC, read_reg(PC)+3))                   \
	F(0x22, "", return -1)                                           \
	F(0x23, "", return -1)                                           \
	F(0x24, "", return -1)                                           \
	F(0x25, "", return -1)                                           \
	F(0x26, "", return -1)                                           \
	F(0x27, "", return -1)                                           \
	F(0x28, "", return -1)                                           \
	F(0x29, "", return -1)                                           \
	F(0x2a, "", return -1)                                           \
	F(0x2b, "", return -1)                                           \
	F(0x2c, "", return -1)                                           \
	F(0x2d, "", return -1)                                           \
	F(0x2e, "", return -1)                                           \
	F(0x2f, "", return -1)                                           \
	F(0x30, "", return -1)                                           \
	F(0x31, "LD SP, nn",                                             \
			write_reg(SP, read_nn());                        \
			set_lclock(12);                                  \
			write_reg(PC, read_reg(PC)+3))                   \
	F(0x32, "LD (HL), A",                                            \
			write_mem(read_reg(HL), read_reg(A)-1);          \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0x33, "", return -1)                                           \
	F(0x34, "", return -1)                                           \
	F(0x35, "", return -1)                                           \
	F(0x36, "", return -1)                                           \
	F(0x37, "", return -1)                                           \
	F(0x38, "", return -1)                                           \
	F(0x39, "", return -1)                                           \
	F(0x3a, "", return -1)                                           \
	F(0x3b, "", return -1)                                           \
	F(0x3c, "", return -1)                                           \
	F(0x3d, "", return -1)                                           \
	F(0x3e, "LD A, n",                                               \
			write_reg(A, read_n());                          \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+2))                   \
	F(0x3f, "", return -1)                                           \
	F(0x40, "", return -1)                                           \
	F(0x41, "", return -1)                                           \
	F(0x42, "", return -1)                                           \
	F(0x43, "", return -1)                                           \
	F(0x44, "", return -1)                                           \
	F(0x45, "", return -1)                                           \
	F(0x46, "", return -1)                                           \
	F(0x47, "", return -1)                                           \
	F(0x48, "", return -1)                                           \
	F(0x49, "", return -1)                                           \
	F(0x4a, "", return -1)                                           \
	F(0x4b, "", return -1)                                           \
	F(0x4c, "", return -1)                                           \
	F(0x4d, "", return -1)                                           \
	F(0x4e, "", return -1)                                           \
	F(0x4f, "", return -1)                                           \
	F(0x50, "", return -1)                                           \
	F(0x51, "", return -1)                                           \
	F(0x52, "", return -1)                                           \
	F(0x53, "", return -1)                                           \
	F(0x54, "", return -1)                                           \
	F(0x55, "", return -1)                                           \
	F(0x56, "", return -1)                                           \
	F(0x57, "", return -1)                                           \
	F(0x58, "", return -1)                                           \
	F(0x59, "", return -1)                                           \
	F(0x5a, "", return -1)                                           \
	F(0x5b, "", return -1)                                           \
	F(0x5c, "", return -1)                                           \
	F(0x5d, "", return -1)                                           \
	F(0x5e, "", return -1)                                           \
	F(0x5f, "", return -1)                                           \
	F(0x60, "", return -1)                                           \
	F(0x61, "", return -1)                                           \
	F(0x62, "", return -1)                                           \
	F(0x63, "", return -1)                                           \
	F(0x64, "", return -1)                                           \
	F(0x65, "", return -1)                                           \
	F(0x66, "", return -1)                                           \
	F(0x67, "", return -1)                                           \
	F(0x68, "", return -1)                                           \
	F(0x69, "", return -1)                                           \
	F(0x6a, "", return -1)                                           \
	F(0x6b, "", return -1)                                           \
	F(0x6c, "", return -1)                                           \
	F(0x6d, "", return -1)                                           \
	F(0x6e, "", return -1)                                           \
	F(0x6f, "", return -1)                                           \
	F(0x70, "", return -1)                                           \
	F(0x71, "", return -1)                                           \
	F(0x72, "", return -1)                                           \
	F(0x73, "", return -1)                                           \
	F(0x74, "", return -1)                                           \
	F(0x75, "", return -1)                                           \
	F(0x76, "", return -1)                                           \
	F(0x77, "", return -1)                                           \
	F(0x78, "", return -1)                                           \
	F(0x79, "", return -1)                                           \
	F(0x7a, "", return -1)                                           \
	F(0x7b, "", return -1)                                           \
	F(0x7c, "LD A, H",                                               \
			write_reg(A, read_reg(H));                       \
			set_lclock(4);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0x7d, "", return -1)                                           \
	F(0x7e, "", return -1)                                           \
	F(0x7f, "", return -1)                                           \
	F(0x80, "", return -1)                                           \
	F(0x81, "", return -1)                                           \
	F(0x82, "", return -1)                                           \
	F(0x83, "", return -1)                                           \
	F(0x84, "", return -1)                                           \
	F(0x85, "", return -1)                                           \
	F(0x86, "", return -1)                                           \
	F(0x87, "", return -1)                                           \
	F(0x88, "", return -1)                                           \
	F(0x89, "", return -1)                                           \
	F(0x8a, "", return -1)                                           \
	F(0x8b, "", return -1)                                           \
	F(0x8c, "", return -1)                                           \
	F(0x8d, "", return -1)                                           \
	F(0x8e, "", return -1)                                           \
	F(0x8f, "", return -1)                                           \
	F(0x90, "", return -1)                                           \
	F(0x91, "", return -1)                                           \
	F(0x92, "", return -1)                                           \
	F(0x93, "", return -1)                                           \
	F(0x94, "", return -1)                                           \
	F(0x95, "", return -1)                                           \
	F(0x96, "", return -1)                                           \
	F(0x97, "", return -1)                                           \
	F(0x98, "", return -1)                                           \
	F(0x99, "", return -1)                                           \
	F(0x9a, "", return -1)                                           \
	F(0x9b, "", return -1)                                           \
	F(0x9c, "", return -1)                                           \
	F(0x9d, "", return -1)                                           \
	F(0x9e, "", return -1)                                           \
	F(0x9f, "SBC A. n",                                              \
			int ncsum = read_reg(A) + read_f(fC);            \
			write_reg(A, read_reg(A) - ncsum);               \
			set_lclock(4);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0xa0, "", return -1)                                           \
	F(0xa1, "", return -1)                                           \
	F(0xa2, "", return -1)                                           \
	F(0xa3, "", return -1)                                           \
	F(0xa4, "", return -1)                                           \
	F(0xa5, "", return -1)                                           \
	F(0xa6, "", return -1)                                           \
	F(0xa7, "", return -1)                                           \
	F(0xa8, "", return -1)                                           \
	F(0xa9, "", return -1)                                           \
	F(0xaa, "", return -1)                                           \
	F(0xab, "", return -1)                                           \
	F(0xac, "", return -1)                                           \
	F(0xad, "", return -1)                                           \
	F(0xae, "", return -1)                                           \
	F(0xaf, "XOR A",                                                 \
			int result = read_reg(A) ^ read_reg(A);          \
			write_reg(A, result);                            \
			write_f(fZ, 1, result == 0);                     \
			write_f(fN, 0, TRUE);                            \
			write_f(fH, 0, TRUE);                            \
			write_f(fC, 0, TRUE);                            \
			set_lclock(4);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0xb0, "", return -1)                                           \
	F(0xb1, "", return -1)                                           \
	F(0xb2, "", return -1)                                           \
	F(0xb3, "", return -1)                                           \
	F(0xb4, "", return -1)                                           \
	F(0xb5, "", return -1)                                           \
	F(0xb6, "", return -1)                                           \
	F(0xb7, "", return -1)                                           \
	F(0xb8, "", return -1)                                           \
	F(0xb9, "", return -1)                                           \
	F(0xba, "", return -1)                                           \
	F(0xbb, "", return -1)                                           \
	F(0xbc, "", return -1)                                           \
	F(0xbd, "", return -1)                                           \
	F(0xbe, "", return -1)                                           \
	F(0xbf, "", return -1)                                           \
	F(0xc0, "", return -1)                                           \
	F(0xc1, "", return -1)                                           \
	F(0xc2, "", return -1)                                           \
	F(0xc3, "", return -1)                                           \
	F(0xc4, "", return -1)                                           \
	F(0xc5, "", return -1)                                           \
	F(0xc6, "", return -1)                                           \
	F(0xc7, "", return -1)                                           \
	F(0xc8, "", return -1)                                           \
	F(0xc9, "", return -1)                                           \
	F(0xca, "", return -1)                                           \
	F(0xcb, "CB",                                                    \
			return EXEC_CBP_INSTR(read_n()))                 \
	F(0xcc, "", return -1)                                           \
	F(0xcd, "", return -1)                                           \
	F(0xce, "", return -1)                                           \
	F(0xcf, "", return -1)                                           \
	F(0xd0, "", return -1)                                           \
	F(0xd1, "", return -1)                                           \
	F(0xd2, "", return -1)                                           \
	F(0xd3, "", return -1)                                           \
	F(0xd4, "", return -1)                                           \
	F(0xd5, "", return -1)                                           \
	F(0xd6, "", return -1)                                           \
	F(0xd7, "", return -1)                                           \
	F(0xd8, "", return -1)                                           \
	F(0xd9, "", return -1)                                           \
	F(0xda, "", return -1)                                           \
	F(0xdb, "", return -1)                                           \
	F(0xdc, "", return -1)                                           \
	F(0xdd, "", return -1)                                           \
	F(0xde, "", return -1)                                           \
	F(0xdf, "", return -1)                                           \
	F(0xe0, "", return -1)                                           \
	F(0xe1, "", return -1)                                           \
	F(0xe2, "LD (C), A",                                             \
			write_mem(0xFF00 + read_reg(C), read_reg(A));    \
			set_lclock(8);                                   \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0xe3, "", return -1)                                           \
	F(0xe4, "", return -1)                                           \
	F(0xe5, "", return -1)                                           \
	F(0xe6, "", return -1)                                           \
	F(0xe7, "", return -1)                                           \
	F(0xe8, "", return -1)                                           \
	F(0xe9, "", return -1)                                           \
	F(0xea, "", return -1)                                           \
	F(0xeb, "", return -1)                                           \
	F(0xec, "", return -1)                                           \
	F(0xed, "", return -1)                                           \
	F(0xee, "", return -1)                                           \
	F(0xef, "", return -1)                                           \
	F(0xf0, "", return -1)                                           \
	F(0xf1, "", return -1)                                           \
	F(0xf2, "", return -1)                                           \
	F(0xf3, "", return -1)                                           \
	F(0xf4, "", return -1)                                           \
	F(0xf5, "", return -1)                                           \
	F(0xf6, "", return -1)                                           \
	F(0xf7, "", return -1)                                           \
	F(0xf8, "", return -1)                                           \
	F(0xf9, "", return -1)                                           \
	F(0xfa, "", return -1)                                           \
	F(0xfb, "EI",                                                    \
			printf("Interrupts enabled. TODO: interrupts."); \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0xfc, "", return -1)                                           \
	F(0xfd, "", return -1)                                           \
	F(0xfe, "CP d8",                                                 \
			int result = read_reg(A) - read_n();             \
			write_f(fZ, 1, result == 0);                     \
			write_f(fN, 1, TRUE);                            \
			write_f(fH, 1, !if_borrow(4, result, 1));        \
	 		write_f(fC, 1, result < 0);                      \
			write_reg(PC, read_reg(PC)+1))                   \
	F(0xff, "RST 38",                                                \
			push(read_reg(PC));                              \
			write_reg(PC, read_n());                         \
			write_reg(PC, read_reg(PC)+1))

#define BASE_INSTR_FUNCS(OP, INAME, CMDS) int exec_base##OP() { CMDS; return 0; }
#define BASE_INSTR_STRUCTS(OP, INAME, CMDS)  { .iname=INAME, .ifunc=exec_base##OP },

BASE_INSTR_TABLE(BASE_INSTR_FUNCS)
instr_t base_instrs[] = { BASE_INSTR_TABLE(BASE_INSTR_STRUCTS) };

#define EXEC_BASE_INSTR(instr) base_instrs[instr].ifunc()
#define PP_INSTR(instr) base_instrs[instr].iname

int step_cpu() {
	int pc = read_reg(PC);
	int instr = read_byte(mem, pc);
	printf("Executing \"%s\" [%#x]. PC is %d. \n", PP_INSTR(instr), instr, pc);
	if (EXEC_BASE_INSTR(instr)) {
		printf("Error: unimplemented instruction.\n");
		return -1;
	};
	//pp_regs();
	return 0;
}
