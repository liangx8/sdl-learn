#pragma once
#include <SDL2/SDL.h>
typedef struct {
    SDL_Renderer    *ren;
    SDL_Window      *win;
    void            *payload;
    SDL_DisplayMode  dm;
    Uint32           switchStageType;
    int              cpunum;
    int              runing;
} RUNSTATE;