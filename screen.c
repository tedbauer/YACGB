#include <stdlib.h>
#include <SDL2/SDL.h>

SDL_Window* window;
SDL_Surface* screen_surface;

int init_screen() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Yacgb", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	screen_surface = SDL_GetWindowSurface(window);
	SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF));
	SDL_UpdateWindowSurface(window);
	return 0;
}

int draw_screen() {
	SDL_UpdateWindowSurface(window);
	return 0;
}

int cleanup_screen() {
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
