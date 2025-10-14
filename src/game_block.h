#pragma once
#include <SDL2/SDL.h>
#define ROW_CNT 25
#define COL_CNT 15
struct BLOCK_DATA{
    Uint8        nextColorIdx;   // 下一个颜色
    Uint8        curColorIdx; // 当前方块颜色
    Uint16       next;           // 下一个方块形状，u32形式
    Uint16       curPos;
    Uint16       oldPos;
    Uint32       timerCount;     // 方块下落的速度1000等于每１秒移动一次
    int          scores;
    // 方块最大由16小块组成 所以是16个位置。当前方块形状，在ground中的偏移位置方式
    Uint8        blocks[16];
    // 图像移动后要消除旧的块。
    Uint8        old[16];
    Uint8        ground[ROW_CNT*COL_CNT];
};
int  game_block_next(void);
void game_block_start(void);
int  game_block_fall(void);
int  game_block_drop(void);
int  game_block_left(void);
int  game_block_right(void);
int  game_block_rotate(Uint8 []);
int  game_block_check_line(void);
void scorestr(char[]);
