#pragma once
#include <SDL2/SDL.h>
#include "cpl.h"
#include "abc_stage.h"
typedef struct _APPRES{
    SDL_Texture         *textureBg;
    SDL_mutex           *mutex;
    SDL_cond            *cond;
    SDL_Rect             rectBg;
    struct TEXTURE_ASCII font_top;
    STAGE               *menu;
    STAGE               *game;
    Uint32               colors[256];
} APPRES;
//主界面水平边界
#define MAIN_SCREEN_MARGIN_H 400
//垂直边界
#define MAIN_SCREEN_MARGIN_V 200
