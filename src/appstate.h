#pragma once
#include <SDL3/SDL.h>
struct APPSTATE{
    SDL_Window   *win;
    SDL_Renderer *render;
    SDL_Texture  *texture;
    SDL_Point winsize;
    uint64_t tick;
    int cpunum;
};