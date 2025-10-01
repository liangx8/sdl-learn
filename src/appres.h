#pragma once
#include <SDL2/SDL.h>
#include "cpl.h"

typedef struct _APPRES{
    SDL_Texture      *texture;
    SDL_mutex        *mutex;
    SDL_cond         *cond;
    SDL_Rect          textureRect;
    struct TEXTURE_ASCII font_top;
    Uint32            colors[256];
} APPRES;
