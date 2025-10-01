#include <malloc.h>
#include <strings.h>
#include "abc_runstate.h"
#include "abc_stage.h"
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
int blockGameStageInit(int row,int col,STAGE *stage){

}
struct GAMESTATE *newGameState(int row,int col)
{
    SDL_Log("sizeof GAMESTATE:%ld,sizeof(int)%ld",sizeof(struct GAMESTATE),sizeof(int));
    struct GAMESTATE *gs=malloc(sizeof(struct GAMESTATE)+row *col);
    gs->row=row;
    gs->col=col;
    bzero(&gs->ground[0],row*col);
    return gs;
}
void releaseGameState(struct GAMESTATE *gs)
{
    free(gs);
}
void fillGround(struct GAMESTATE *gs)
{
}

int drawGround(struct GAMESTATE *gs,SDL_Renderer *ren)
{
    return 0;
}