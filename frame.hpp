#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include <unordered_map>
#include <forward_list>
#include "structs.hpp"

int frame(std::unordered_map<int, int> *colors, 
            std::forward_list<Wall> *walls,
            std::forward_list<Ray> *rays);