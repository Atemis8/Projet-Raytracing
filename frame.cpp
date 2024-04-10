#include "frame.hpp"
#include "structs.hpp"
#include "imguitest.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <iostream>


#define SIZEX 600.0F
#define SIZEY 600.0F

#define ORIGINX 100.0F
#define ORIGINY 100.0F

#define RAY_COLOR 0xFF0000FF
#define BACKGROUND_COLOR 0x000000FF

#define RED 0xFF0000FF
#define GREEN 0x00FF00FF
#define BLUE 0x0000FFFF

#define SCALE 4
#define SCALE_POINT 4

void setColor(SDL_Renderer* rend, int color) {
	const int red = (color & 0xFF000000) >> 24;
	const int green = (color & 0x00FF0000) >> 16;
	const int blue = (color & 0x0000FF00) >> 8;
	const int alpha = (color & 0x000000FF);
	SDL_SetRenderDrawColor(rend, red, green, blue, alpha);
}

PosVector setAxis(PosVector *v) {
	return PosVector(ORIGINX +  v->getX() * SCALE, SIZEY - ORIGINY - v->getY() * SCALE);
}

void drawLine(SDL_Renderer* rend, PosVector *v1, PosVector *v2) {
	const PosVector v3 = setAxis(v1);
	const PosVector v4 = setAxis(v2);
	SDL_RenderDrawLine(rend, v3.getX(), v3.getY(), v4.getX(), v4.getY());
}

void drawPoint(SDL_Renderer* rend, PosVector *p) {
	const PosVector pa = setAxis(p);
	SDL_RenderSetScale(rend, SCALE_POINT, SCALE_POINT);
	SDL_RenderDrawPoint(rend, pa.getX() / SCALE_POINT, pa.getY() / SCALE_POINT);
	SDL_RenderSetScale(rend, 1.0, 1.0);
}


int frame(std::unordered_map<int, int> *colors, 
			std::forward_list<Wall> *walls, 
			std::forward_list<Ray> *rays, 
			std::forward_list<PosVector> *debug_points) {
	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

	// Creates a window
	SDL_Window* win = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIZEX, SIZEY, 0);
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
	SDL_RenderSetLogicalSize(rend, 500, 500);

	setColor(rend, BACKGROUND_COLOR);
	SDL_RenderClear(rend); 
	setColor(rend, BLUE);
	PosVector origin = PosVector(0, 0);
	drawPoint(rend, &origin);

	for(Wall w : *walls) {
		setColor(rend, colors->at(w.mat.id));
		drawLine(rend, &w.v1, &w.v2);
	}


	PosVector *temp = (PosVector*) malloc(sizeof(PosVector)); 
	for(Ray& r : *rays) {
		int i = 1;
		for(PosVector& v : r.points) { 
			if(i) { temp = &v; i = 0; continue; }
			setColor(rend, r.color);
		 	drawLine(rend, temp, &v); temp = &v;
		}
	}

	setColor(rend, GREEN);
	for(PosVector dp : *debug_points) {
		drawPoint(rend, &dp);
	}

    SDL_RenderPresent(rend);

	int x, y;
	SDL_GetWindowSize(win, &x, &y);

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
    SDL_Delay(1000 / 60); // calculates to 60 fps
	}

	SDL_DestroyRenderer(rend); // destroy renderer
	SDL_DestroyWindow(win); // destroy window
	SDL_Quit(); // close SDL

	return 0;
}
