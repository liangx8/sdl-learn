#pragma once
#include <SDL2/SDL.h>
typedef struct _RUNSTATE{
    SDL_Renderer    *ren;
    SDL_Window      *win;
    void            *payload;
    SDL_DisplayMode  dm;
    int              winw;
    int              winh;
    int              cpunum;
    int              present;
    int              runing;
} RUNSTATE;