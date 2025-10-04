#pragma once
#include <SDL2/SDL.h>
#define ROW_CNT 25
#define COL_CNT 15
struct BLOCK_DATA{
    Uint8        nextColorIdx;   // 下一个颜色
    Uint8        blocksColorIdx; // 当前方块颜色
    Uint16       next;           // 下一个方块形状，u32形式
    // 方块最大由８小块组成 所以是8个位置。当前方块形状，在ground中的偏移位置方式
    Uint16       blocks[8];
    Uint8        ground[ROW_CNT*COL_CNT];
};

