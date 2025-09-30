#pragma once
#include <SDL2/SDL.h>
#include "stage.h"
typedef struct {
    SDL_Renderer    *ren;
    SDL_Window      *win;
    void            *payload;
    SDL_DisplayMode  dm;
    int              cpunum;
    int              screenUpdate;
    int              runing;
} RUNSTATE;