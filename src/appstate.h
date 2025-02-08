#pragma once
#include <SDL2/SDL.h>
#include "cpl.h"
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
    SDL_Window       *win;
    SDL_Renderer     *render;
    SDL_Texture      *texture;
    SDL_mutex        *mutex;
    struct GAMESTATE *gs;
    SDL_DisplayMode   dm;
    SDL_Rect          textureRect;
    struct TEXTURE_ASCII font_top;
    Uint64            num;
    Uint32            colors[256];
    int               cpunum;
    // 数据已经有改变。需要更新到屏幕
    int             screenOut;
    int             runing;
};