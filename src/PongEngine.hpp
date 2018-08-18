#pragma once

#include "Window.hpp"

#include <SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

struct PongEngine
{
    isc::Window window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
};
