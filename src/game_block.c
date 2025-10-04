#include <wchar.h>
#include "game_block.h"
#if 0
struct BLOCK_DATA{
    Uint8        nextColorIdx;   // 下一个颜色
    Uint8        blocksColorIdx; // 当前方块颜色
    Uint16       next;           // 下一个方块形状，u32形式
    // 方块最大由８小块组成 所以是8个位置。当前方块形状，在ground中的偏移位置方式
    Uint16       blocks[8];
    Uint8        ground[ROW_CNT*COL_CNT];
};
#endif
struct BLOCK_DATA bd;
const Uint16 block_template[]={
    0b00001111, // line
    0b00100111, // T
    0b00010111, // left L
    0b01000111, // right L
    0b00110011, // squard
    0b00110110, // left Z
    0b01100011  // right Z
};
int gen_next(void)
{
    bd.next=block_template[rand() % 6];
    bd.nextColorIdx=rand() & 0xff;

#if 0
    for(int ix=0;ix<7;ix++){
        Uint16 type=block_template[ix];
        for(int ia=0;ia<4;ia++){
            for(int ib=0;ib<4;ib++){
                if(type&1){
                    putwchar(L'X');
                } else {
                    putwchar(L' ');
                }
                type = type >> 1;
            }
            putwchar(L'\n');
        }
    }
#endif
    return 0;
}