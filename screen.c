#include <stdlib.h>
#include <SDL2/SDL.h>
#include "assert.h"

const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 144;

SDL_Window* window;
SDL_Renderer *renderer;

int init_screen() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		"Yacgb",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL
	);
	renderer = SDL_CreateRenderer(window, -1, 0);
	return 0;
}

int draw_screen(int pmap[160][144]) {
	for (int i = 0; i < 160; i++) {
		for (int j = 0; j < 144; j++) {
			switch (pmap[i][j]) {
				case 0:
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					break;
				case 1:
					SDL_SetRenderDrawColor(renderer, 192, 192, 192, 192);
					break;
				case 2:
					SDL_SetRenderDrawColor(renderer, 96, 96, 96, 96);
					break;
				case 3:
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
					break;
				default:
					assert(0);
			}
			SDL_RenderDrawPoint(renderer, i, j);
		}
	}
	SDL_RenderPresent(renderer);
	return 0;
}

int cleanup_screen() {
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
