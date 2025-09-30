#pragma once
#include <SDL2/SDL.h>
#include "cpl.h"
#include "stage.h"
struct GAMESTATE{
    Uint32 bgcolor;
    Uint8  nextColorIdx;   // 下一个颜色
    Uint8  blocksColorIdx; // 当前方块颜色
    Uint16 next;           // 下一个方块形状，u32形式
    Uint16 blocks[8];      // 当前方块形状，在ground中的偏移位置方式
    Uint32 row;
    Uint32 col;
    Uint8  ground[0];
};
struct APPSTATE{
    SDL_Texture      *texture;
    SDL_mutex        *mutex;
    SDL_cond         *cond;
    struct GAMESTATE *gs;
    SDL_Rect          textureRect;
    struct TEXTURE_ASCII font_top;
    Uint32            colors[256];
    RUNSTATE          *rs;
};
