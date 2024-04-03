#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

int main(int argc, char *argv[])
{

	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}

  // Creates a window
	SDL_Window* win = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, 0);
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

	int close = 0;
	while (!close) {

    // Events management
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {

        // handling of close button
        case SDL_QUIT:
          close = 1;
          break;
			}
		}
    // Renders Background
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend); 

    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderDrawLine(rend, 100, 100, 500, 100);
		SDL_RenderPresent(rend); // triggers the double buffers for multiple rendering
		SDL_Delay(1000 / 60); // calculates to 60 fps
	}
	
	SDL_DestroyRenderer(rend); // destroy renderer
	SDL_DestroyWindow(win); // destroy window
	SDL_Quit(); // close SDL

	return 0;
}
