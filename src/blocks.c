#include <malloc.h>
#include <strings.h>
#include "appstate.h"


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