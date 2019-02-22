#include "mem.h"
#include "assert.h"

typedef enum {
	M_HBLANK  =0,
	M_VBLANK  =1,
	M_OAMSCAN =2,
	M_VRAMSCAN=3
} mode_t;

const int HBLANK_CYCLES  = 204;
const int VBLANK_CYCLES  = 4560;
const int OAM_CYCLES     = 80;
const int VRAM_CYCLES    = 172;

const int NUM_LINES = 144;

mode_t mode;
int gpu_clock;
int screen_line;
mem_state_t* mem;

int init_gpu(mem_state_t* new_mem) {
	mem         = new_mem;
	gpu_clock   = 0;
	screen_line = 0;
	return 0;
}

int step_gpu(int cycles) {
	gpu_clock += cycles;
	switch(mode) {
		case M_HBLANK:
			if (cycles >= HBLANK_CYCLES) {
				screen_line += 1;
				gpu_clock = 0;
				if (screen_line >= NUM_LINES) {
					/* TODO: Write framebuffer to screen! */
					assert(0);
					mode = M_VBLANK;
				} else {
					mode = M_OAMSCAN;
				}
			}
			break;
		case M_VBLANK:
			if (cycles >= VBLANK_CYCLES) {
				gpu_clock   = 0;
				screen_line = 0;
				mode = M_OAMSCAN;
			}
			break;
		case M_OAMSCAN:
			if (cycles >= OAM_CYCLES) {
				gpu_clock = 0;
				mode = M_VRAMSCAN;
			}
			break;
		case M_VRAMSCAN:
			if (cycles >= VRAM_CYCLES) {
				gpu_clock = 0;
				mode = M_HBLANK;
				/* FIXME: do scanline stuff */
				assert(0);
			}
			break;
		default:
			assert(0);
	}
	return 0;
}
