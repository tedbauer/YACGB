#include <stdlib.h>

#define IN_RANGE(addr, start, end) (addr >= start && addr <= end)

typedef struct {
	int bios_stat;
	int bios[256];
	int rom_bank0[16384];
	int rom_bank1[16384];
	int gpu_vram[8192];
	int ext_ram[8192];
	int working_ram[16384];
	int gfx_sinfo[160];
	int mm_io[128];
	int z_ram[128];
} mem_state_t;

mem_state_t* mem_init() {
	return malloc(sizeof(mem_state_t));
}

void mem_cleanup(mem_state_t* mem) {
	free(mem);
}

int read_byte(mem_state_t* mem, int addr) {

	// ROM bank 0
	if IN_RANGE(addr, 0x0000, 0x3FFF) {

		// BIOS
		if IN_RANGE(addr, 0x0000, 0x00FF) {
			if (mem->bios_stat) {
				return mem->bios[addr];
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
