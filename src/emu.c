#include <stdio.h>
#include "../include/cpu.h"
#include "../include/mem.h"
#include "../include/screen.h"
#include "../include/gpu.h"
#include <SDL2/SDL.h>

int main() {
	mem_state_t* mem = init_mem();
	init_cpu(mem);
	init_screen();

	SDL_Event e;

	while (1) {
		while(SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				return 0;
			}
		}
		step_cpu();
		step_gpu();
	}

	return 0;

	/*
	while (1) {
		if (step_cpu()) {
			return -1;
		}
	}
	return 0;
	*/
}
