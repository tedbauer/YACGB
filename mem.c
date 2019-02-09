#include <stdlib.h>
#include <stdio.h>

#define IN_RANGE(addr, start, end) (addr >= start && addr <= end)

char* bios[] = {
	"31", "fe", "ff", "af", "21", "ff", "9f", "32", "cb",
	"7c", "20", "fb", "21", "26", "ff", "0e", "11", "3e",
	"80", "32", "e2", "0c", "3e", "f3", "e2", "32", "3e",
	"77", "77", "3e", "fc", "e0", "47", "21", "04", "01",
	"e5", "11", "cb", "00", "1a", "13", "be", "20", "6b",
	"23", "7d", "fe", "34", "20", "f5", "06", "19", "78",
	"86", "23", "05", "20", "fb", "86", "20", "5a", "d1",
	"21", "10", "80", "1a", "cd", "a9", "00", "cd", "aa",
	"00", "13", "7b", "fe", "34", "20", "f3", "3e", "18",
	"21", "2f", "99", "0e", "0c", "32", "3d", "28", "09",
	"0d", "20", "f9", "11", "ec", "ff", "19", "18", "f1",
	"67", "3e", "64", "57", "e0", "42", "3e", "91", "e0",
	"40", "04", "1e", "02", "cd", "bc", "00", "0e", "13",
	"24", "7c", "1e", "83", "fe", "62", "28", "06", "1e",
	"c1", "fe", "64", "20", "06", "7b", "e2", "0c", "3e",
	"87", "e2", "f0", "42", "90", "e0", "42", "15", "20",
	"dd", "05", "20", "69", "16", "20", "18", "d6", "3e",
	"91", "e0", "40", "1e", "14", "cd", "bc", "00", "f0",
	"47", "ee", "ff", "e0", "47", "18", "f3", "4f", "06",
	"04", "c5", "cb", "11", "17", "c1", "cb", "11", "17",
	"05", "20", "f5", "22", "23", "22", "23", "c9", "0e",
	"0c", "f0", "44", "fe", "90", "20", "fa", "0d", "20",
	"f7", "1d", "20", "f2", "c9", "ce", "ed", "66", "66",
	"cc", "0d", "00", "0b", "03", "73", "00", "83", "00",
	"0c", "00", "0d", "00", "08", "11", "1f", "88", "89",
	"00", "0e", "dc", "cc", "6e", "e6", "dd", "dd", "d9",
	"99", "bb", "bb", "67", "63", "6e", "0e", "ec", "cc",
	"dd", "dc", "99", "9f", "bb", "b9", "33", "3e", "ff",
	"ff", "3c", "e0", "50"
};

typedef struct {
	int bios_stat;
	char** bios;
	int rom_bank0[16384];
	int rom_bank1[16384];
	int gpu_vram[8192];
	int ext_ram[8192];
	int working_ram[16384];
	int gfx_sinfo[160];
	int mm_io[128];
	int z_ram[128];
} mem_state_t;

mem_state_t* init_mem() {
	mem_state_t* new_mem = malloc(sizeof(mem_state_t));
	new_mem->bios_stat = 1;
	new_mem->bios = bios;
	return new_mem;
}

void cleanup_mem(mem_state_t* mem) {
	free(mem);
}

int read_byte(mem_state_t* mem, int addr) {

	// ROM bank 0
	if IN_RANGE(addr, 0x0000, 0x3FFF) {

		// BIOS
		if IN_RANGE(addr, 0x0000, 0x00FF) {
			if (mem->bios_stat) {
				printf("Read from BIOS.\n");
				return (int)mem->bios[addr];
			}
		} else {
			return mem->rom_bank0[addr];
		}
	}

	// ROM bank 1
	else if IN_RANGE(addr, 0x4000, 0x7FFF) {
		return mem->rom_bank1[addr - 0x4000];
	}

	// Graphics RAM
	else if IN_RANGE(addr, 0x8000, 0x9FFF) {
		return mem->gpu_vram[addr - 0x8000];
	}

}

char* write_byte(mem_state_t* mem, uint8_t addr, uint8_t val);
