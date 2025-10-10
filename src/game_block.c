#include <wchar.h>
#include "game_block.h"


/*
0  1  2  3
4  5  6  7
8  9  10 11
12 13 14 15

12 8  4  0
13 9  5  1
14 10 6  2
15 11 7  3


*/


const char rotation_matrix[16]={
    3 , 6, 8,12,
    -2, 1, 4, 7,
    -7,-4,-1, 2,
    -12,-9,-6,-3
};

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
 * @brief 把分数转化成字符串
 */
void scorestr(char str[]){
    snprintf(str,8,"%07d",bd.scores);
}
/**
 * @brief 固定块到游戏区
 */
void game_block_fix(void)
{
    
    for(int ix=0;ix<16;ix++){
        int pos=bd.blocks[ix];
        if(pos==0)break;
        pos--;
        bd.ground[pos]=bd.curColorIdx;
    }
}
int check_pos(Uint8 blocks[],int pos)
{

}
/**
 * @brief 活动方块向下移动一格
 * @return -1有东西阻塞，不能移动
 * @todo completed, but not test yet
 */
int game_block_fall(void)
{
    int pos=bd.curPos + COL_CNT;
    for(int iy=0;iy<4;iy++){
        for(int ix=0;ix<4;ix++){
            int idx=bd.blocks[iy*4+ix];

        }
    }
    return 0;
}
/**
 * @brief 左移动
 * @return 0 左移动成功, -1被堵住
 */
int game_block_left(void)
{
    Uint16 blocks[16];
    SDL_memset4(blocks,0,8);
    for(int ix=0;ix<16;ix++){
        int min;
        int pos=bd.blocks[ix];
        if(pos==0){
            break;
        }
        //取实际位置
        pos--;
        //当前行的最小值
        min=pos / COL_CNT;
        min = min * COL_CNT;
        pos--;
        if((pos < min)||(bd.ground[pos])){
            return -1;
        }
        blocks[ix]=pos+1;
    }
    SDL_memcpy4(bd.old,bd.blocks,8);
    SDL_memcpy4(bd.blocks,blocks,8);
    return 0;
}  
/**
 * @brief 右移动
 * @return 0 移动成功, -1被堵住
 */
int  game_block_right(void)
{
    Uint16 blocks[16];
    SDL_memset4(blocks,0,8);
    for(int ix=0;ix<16;ix++){
        int max;
        int pos=bd.blocks[ix];
        if(pos==0){
            break;
        }
        //取实际位置
        pos--;
        //当前行的最小值
        max=pos / COL_CNT;
        max = (max+1) * COL_CNT;
        pos++;
        if((pos >= max)||(bd.ground[pos])){
            return -1;
        }
        blocks[ix]=pos+1;
    }
    SDL_memcpy4(bd.old,bd.blocks,8);
    SDL_memcpy4(bd.blocks,blocks,8);
    return 0;
}
int  game_block_rotate(void)
{
    Uint16 blocks[16];
    SDL_memset4(blocks,0,8);
    int basex=1000,basey=1000;
    for(int ix=0;ix<16;ix++){
        int pos=bd.blocks[ix];
        if(pos==0){
            break;
        }
        pos --;
        int x=pos % COL_CNT;
        if(x<basex){
            basex=x;
        }
        int y=pos / COL_CNT;
        if(y<basey){
            basey=y;
        }
    }
    int baseidx=basey*COL_CNT+basex;
    return 0;

}

/**
 * @brief 生成新组块，并且把旧快放到blocks中，
 * @return -1 放不下了，0 OK
 */
int game_block_next(void)
{
    Uint16 cur=bd.next;
    bd.curColorIdx=bd.nextColorIdx;

    bd.next=block_template[rand() % 7];
    bd.nextColorIdx=(rand() & 0xff)+1;
    SDL_memset4(bd.blocks,0,4);
    Uint8 idx=0;
        for(int ix=0;ix<16;ix++){
            if(cur==0){
                break;
            }
            if(cur & 1){
                bd.blocks[idx]=ix+1;
                idx++;
            }
            cur = cur >> 1;
    }
    bd.curPos=COL_CNT/2-2;
    SDL_memset4(bd.old,0,4);
    return 0;
}
void game_block_start(void){
    SDL_memset(bd.ground,0,ROW_CNT*COL_CNT);
    bd.next=block_template[rand() % 7];
    // eliminate zero;
    bd.nextColorIdx=(rand() & 0xfe)+1;
    game_block_next();
    bd.scores=0;
}

