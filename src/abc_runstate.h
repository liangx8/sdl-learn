#pragma once
#include <SDL2/SDL.h>
typedef struct _RUNSTATE{
    SDL_Renderer    *ren;
    SDL_Window      *win;
    void            *payload;
    SDL_Joystick    *joys;
    SDL_DisplayMode  dm;
    int              winw;
    int              winh;
    int              cpunum;
    int              present;
    volatile int     runing;
} RUNSTATE;