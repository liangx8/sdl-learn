#include <wchar.h>
#include "game_block.h"
#if 1
void showblock(const wchar_t *msg,Uint16 bs[16])
{
    wprintf(msg);
    for(int ix=0;ix<16;ix++){
        if(bs[ix]==0){
            break;
        }
        wprintf(L"%3d ",bs[ix]-1);
    }
    putwchar(L'\n');
}
#endif
#if 0
struct BLOCK_DATA{
    Uint8        nextColorIdx;   // 下一个颜色
    Uint8        blocksColorIdx; // 当前方块颜色
    Uint16       next;           // 下一个方块形状，u32形式
    // 方块最大由８小块组成 所以是8个位置。当前方块形状，在ground中的偏移位置方式
    Uint16       blocks[8];
    // 图像移动后要消除旧的块。
    Uint16       old[16];
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
/**
 * @brief 活动方块向下移动一格
 * @return -1有东西阻塞，不能移动
 * @todo completed, but not test yet
 */
int game_block_fall(void)
{
    Uint16 blocks[16];
    SDL_memset4(blocks,0,8);
    for(int ix=0;ix<16;ix++){
        int pos=bd.blocks[ix];
        if(pos==0){
            break;
        }
        pos = pos +COL_CNT-1;

        if((bd.ground[pos]) || (pos > ROW_CNT * COL_CNT)){
            //触碰到底部，-1退出
            return -1;
        }
        blocks[ix]=pos+1;
    }
    showblock(L"cur: ",bd.blocks);
    showblock(L"old: ",bd.old);
    SDL_memcpy4(bd.old,bd.blocks,8);
    SDL_memcpy4(bd.blocks,blocks,8);
    return 0;
}
void game_block_fix(void)
{
    
}
/**
 * @brief 生成新组块，并且把旧快放到blocks中，
 * @return -1 放不下了，0 OK
 */
int game_block_next(void)
{
    Uint16 cur=bd.next;
    bd.blocksColorIdx=bd.nextColorIdx;

    bd.next=block_template[rand() % 7];
    bd.nextColorIdx=rand() & 0xff;
    SDL_memset(bd.blocks,0,32);
    int idx=0;
    for(int iy=0;iy<4;iy++){
        for(int ix=0;ix<4;ix++){
            if(cur==0){
                goto outer_loop;
            }
            if(cur & 1){
                int pos=iy*COL_CNT+ix + COL_CNT/2 -2;
                if(bd.ground[pos]){
                    return -1;
                }
                // 
                bd.blocks[idx]=pos+1;
                idx++;
            }
            cur = cur >> 1;
        }
    }
    outer_loop:
    SDL_memcpy4(bd.old,bd.blocks,8);
    return 0;
}
void game_block_start(void){
    SDL_memset(bd.ground,0,ROW_CNT*COL_CNT);
    bd.next=block_template[rand() % 7];
    bd.nextColorIdx=rand() & 0xff;
    game_block_next();
    showblock(L"scur:",bd.blocks);
    showblock(L"sold:",bd.old);
}
