#include <stdlib.h>
#include <SDL2/SDL.h>

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
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	/*
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	int i;
	for (i = 0; i < SCREEN_WIDTH; i++) {
		SDL_RenderDrawPoint(renderer, i, i);
	}
	SDL_RenderPresent(renderer);
	*/
	return 0;
}

int draw_screen() {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	int i;
	for (i = 0; i < SCREEN_WIDTH; i++) {
		SDL_RenderDrawPoint(renderer, i, i);
	}
	SDL_RenderPresent(renderer);
	return 0;
}

int cleanup_screen() {
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
