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

typedef union {
	uint8_t bit8;
	uint16_t bit16;
} mem_t;

void update_preg(reg_t r, mem_t new_val) {
	switch(r) {
		case AF:
			cpu->registers[A] = new_val.bit16;
			break;
		case BC:
			cpu->registers[B] = new_val.bit16;
			break;
		case DE:
			cpu->registers[D] = new_val.bit16;
			break;
		case HL:
			cpu->registers[H] = new_val.bit16;
			break;
		default:
			perror("Non-pair supplied to update_preg.");
	}
}

/* Place contents of new_val in register r. */
void update_reg(reg_t r, mem_t new_val) {
	switch(r) {
		case PC:
		case SP:
			cpu->registers[r] = new_val.bit16;
			break;
		case AF:
		case BC:
		case DE:
		case HL:
			update_preg(r, new_val);
			break;
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

/* FIXME */
mem_t incr(mem_t val) {
	mem_t new_val = val;
	new_val.bit8  += 1;
	new_val.bit16 += 1;
	/* FIXME dont do this; augment the mem_t type */
	return new_val;
}

mem_t decr(mem_t val) {
	mem_t new_val = val;
	new_val.bit8  -= 1;
	new_val.bit16 -= 1;
	/* FIXME dont do this; augment the mem_t type */
	return new_val;
}

void exec_instr(char* instr) {

	if (strcmp(instr, "00") == 0)  //NOP
	{
	}
	else if (strcmp(instr, "01") == 0) //LD BC, nn
	{
		update_reg(BC, read_nn());
		set_lclock(12);
	}
	else if (strcmp(instr, "02") == 0) //LD (BC), A
	{
		update_reg(BC, read_reg(A));
		set_lclock(8);
	}
	else if (strcmp(instr, "03") == 0) //INC BC
	{
		update_reg(BC, incr(read_reg(BC)));
		set_lclock(8);
	}
	else if (strcmp(instr, "04") == 0) //INC B
	{
		update_reg(B, incr(read_reg(B)));
		set_lclock(4);
	}
	else if (strcmp(instr, "05") == 0) //DEC B
	{
		update_reg(B, decr(read_reg(B)));
		set_lclock(4);
	}
	else if (strcmp(instr, "06") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "07") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "08") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "09") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "10") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "11") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "12") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "13") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "14") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "15") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "16") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "17") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "18") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "19") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "20") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "21") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "22") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "23") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "24") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "25") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "26") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "27") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "28") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "29") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "30") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "31") == 0)
	{
		uint8_t first_byte  = mem.read_byte(sp+1);
		uint8_t second_byte = mem.read_byte(sp+2);

		cpu->sp =
	}
	else if (strcmp(instr, "32") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "33") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "34") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "35") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "36") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "37") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "38") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "39") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "40") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "41") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "42") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "43") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "44") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "45") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "46") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "47") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "48") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "49") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "50") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "51") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "52") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "53") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "54") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "55") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "56") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "57") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "58") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "59") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "60") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "61") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "62") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "63") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "64") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "65") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "66") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "67") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "68") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "69") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "70") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "71") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "72") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "73") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "74") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "75") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "76") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "77") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "78") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "79") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "80") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "81") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "82") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "83") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "84") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "85") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "86") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "87") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "88") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "89") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "90") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "91") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "92") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "93") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "94") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "95") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "96") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "97") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "98") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "99") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "100") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "101") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "102") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "103") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "104") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "105") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "106") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "107") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "108") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "109") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "110") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "111") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "112") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "113") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "114") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "115") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "116") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "117") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "118") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "119") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "120") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "121") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "122") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "123") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "124") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "125") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "126") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "127") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "128") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "129") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "130") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "131") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "132") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "133") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "134") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "135") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "136") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "137") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "138") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "139") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "140") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "141") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "142") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "143") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "144") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "145") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "146") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "147") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "148") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "149") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "150") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "151") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "152") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "153") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "154") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "155") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "156") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "157") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "158") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "159") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "160") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "161") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "162") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "163") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "164") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "165") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "166") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "167") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "168") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "169") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "170") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "171") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "172") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "173") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "174") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "175") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "176") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "177") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "178") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "179") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "180") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "181") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "182") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "183") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "184") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "185") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "186") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "187") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "188") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "189") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "190") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "191") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "192") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "193") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "194") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "195") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "196") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "197") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "198") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "199") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "200") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "201") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "202") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "203") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "204") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "205") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "206") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "207") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "208") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "209") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "210") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "211") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "212") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "213") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "214") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "215") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "216") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "217") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "218") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "219") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "220") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "221") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "222") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "223") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "224") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "225") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "226") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "227") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "228") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "229") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "230") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "231") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "232") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "233") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "234") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "235") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "236") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "237") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "238") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "239") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "240") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "241") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "242") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "243") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "244") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "245") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "246") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "247") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "248") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "249") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "250") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "251") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "252") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "253") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "254") == 0)
	{
		perror(instr);
	}
	else if (strcmp(instr, "255") == 0)
	{
		perror(instr);
	}


}

int step_cpu() {
	char* instr = mem.read_byte(cpu->pc);
	exec_instr(instr);
}
