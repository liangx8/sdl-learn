#include <malloc.h>
#include <strings.h>
#include "abc.h"
#include "appres.h"
#include "map.h"
#include "cpl.h"
#include "game_block.h"

#define ROW_CNT 25
#define COL_CNT 15
#define GAME_GROUND_MARGIN 10
struct GAMESTATE{
    RUNSTATE *rs;
    // 游戏区的尺寸
    SDL_Texture *blankBackground;
    SDL_Texture *blankNextPreview;
    SDL_TimerID  timerId;
    SDL_Rect     gameLayer;
    SDL_Rect     previewLayer;
    SDL_Color    bgcolor;
    int          gameStatus;
    int          blocksize;
} gs;
#define GS_GAMEERROR      -1
#define GS_GAMEOK         0
#define GS_TIMERFALL      1
#define GS_GAMEOVER       2

#define SCORES_TEXT_POS_X 10
#define SCORES_TEXT_POS_Y 90
extern struct BLOCK_DATA bd;
int play_update(void)
{
    APPRES *aps=(APPRES*)gs.rs->payload;
    int basex,basey;
    Uint32 color;
    SDL_Rect dst;
    dst.w=gs.blocksize-1;
    dst.h=gs.blocksize-1;
    color=aps->colors[bd.blocksColorIdx];
    basex=gs.gameLayer.x+1;
    basey=gs.gameLayer.y+1;
    SDL_Rect src;
    src.h=dst.h;
    src.w=dst.w;
    src.x=1;
    src.y=1;
    // 抹除旧块
    for(int ix=0;ix<16;ix++){
        int pos=bd.old[ix];
        if(pos==0)break;
        pos--;
        int x=pos % COL_CNT;
        int y=pos / COL_CNT;
        dst.x=basex+x*gs.blocksize;
        dst.y=basey+y*gs.blocksize;
        MINUS_ERR(SDL_RenderCopy(gs.rs->ren,gs.blankBackground,&src,&dst))
    }
    // 画新块
    MINUS_ERR(SDL_SetRenderDrawColor(
        gs.rs->ren,
        color & 0xff,
        (color >> 8) & 0xff,
        (color >> 16) & 0xff,
        (color >> 24) & 0xff))
    for(int ix=0;ix<16;ix++){
        int pos=bd.blocks[ix];
        if(pos==0){
            break;
        }
        pos --;
        int x=pos % COL_CNT;
        int y=pos / COL_CNT;
        dst.x=basex+x*gs.blocksize;
        dst.y=basey+y*gs.blocksize;
        MINUS_ERR(SDL_RenderFillRect(gs.rs->ren,&dst))
    }
    SDL_memset4(bd.old,0,8);
    MINUS_ERR(SDL_RenderCopy(gs.rs->ren,gs.blankBackground,&dst,&gs.gameLayer))
    //显示得分
    char ss[8];
    scorestr(ss);
    // 先清背景
    dst.x=GAME_GROUND_MARGIN+SCORES_TEXT_POS_X+80;
    dst.y=SCORES_TEXT_POS_Y;
    src.h=dst.h=30;// 只要大于字体高度就可以
    src.w=dst.w=100;
    src.x=dst.x-aps->rectBg.x;// 边的宽度
    src.y=dst.y-aps->rectBg.y;
    MINUS_ERR(SDL_RenderCopy(gs.rs->ren,aps->textureBg,&src,&dst))
    MINUS_ERR(cpl_render_ascii(gs.rs->ren,&aps->font_top,ss,dst.x,dst.y))
    // SDL_Event ev;
    // ev.type=SDL_WINDOWEVENT;
    // ev.window.event=SDL_WINDOWEVENT_EXPOSED;
    // SDL_PushEvent(&ev);
    gs.rs->present=1;
    return 0;
}
Uint32 blg_timer(Uint32 intv,void *_)
{
    SDL_Event ev;
    ev.type=SDL_USEREVENT;
    ev.user.code=GS_TIMERFALL;
    SDL_PushEvent(&ev);
    return bd.timerCount;
}
int preview_update(void)
{
    Uint16 type=bd.next;
    APPRES *aps=(APPRES*)gs.rs->payload;
    MINUS_ERR(SDL_RenderCopy(gs.rs->ren,gs.blankNextPreview,NULL,&gs.previewLayer))
    Uint32 color=aps->colors[bd.nextColorIdx];
    SDL_Rect rect;
    int basex,basey;
    basex=gs.previewLayer.x+1;
    basey=gs.previewLayer.y+1;
    rect.w=gs.blocksize-1;
    rect.h=gs.blocksize-1;
    MINUS_ERR(SDL_SetRenderDrawColor(
        gs.rs->ren,
        color & 0xff,
        (color >> 8) & 0xff,
        (color >> 16) & 0xff,
        (color >> 24) & 0xff))
    for(int ix=0;ix<4;ix++){
        for(int iy=0;iy<4;iy++){
            if(type==0){
                goto outer_break;
            }
            rect.x=basex + iy * gs.blocksize;
            rect.y=basey;
            if(type & 1){
                MINUS_ERR(SDL_RenderFillRect(gs.rs->ren,&rect))
            }
            type = type >>1;
        }
        basey += gs.blocksize;
    }
    outer_break:
    return play_update();
}
int block_game_return_menu(void *_)
{
    APPRES *aps=(APPRES *)gs.rs->payload;
    MINUS_ERR(switch_stage(aps->menu))
    return 0;
}
int block_game_down(void *_)
{
    int res;
    if(gs.gameStatus==GS_GAMEOVER){
        return 0;
    }
    LOCK_MUTEX();
    res=game_block_fall();
    UNLOCK_MUTEX();
    if(res){
        wprintf(L"手动・触底\n");
        LOCK_MUTEX();
        res=game_block_next();
        UNLOCK_MUTEX();
        if(res){
            wprintf(L"ＧＡＭＥ　ＯＶＥＲ！\n");
            gs.gameStatus=GS_GAMEOVER;
            SDL_RemoveTimer(gs.timerId);
            gs.timerId=0;
        }
        MINUS_ERR(preview_update())
        return 0;
    }
    MINUS_ERR(play_update())
    return 0;
}
int block_game_left(void *_)
{
    int res;
    if(gs.gameStatus==GS_GAMEOVER){
        return 0;
    }
    LOCK_MUTEX();
    res=game_block_left();
    UNLOCK_MUTEX();
    if(res){
        return 0;
    }
    MINUS_ERR(play_update())
    return 0;
}
int block_game_right(void *_)
{
    int res;
    if(gs.gameStatus==GS_GAMEOVER){
        return 0;
    }
    LOCK_MUTEX();
    res=game_block_right();
    UNLOCK_MUTEX();
    if(res){
        return 0;
    }
    MINUS_ERR(play_update())
    return 0;
}
int blg_action_attach(MAP map)
{
    APPRES *aps=(APPRES *)gs.rs->payload;
    gs.gameStatus=GS_GAMEOK;
    MINUS_ERR(SDL_RenderCopy(gs.rs->ren,gs.blankBackground,NULL,&gs.gameLayer))
    MINUS_ERR(SDL_RenderCopy(gs.rs->ren,gs.blankNextPreview,NULL,&gs.previewLayer))
    MINUS_ERR(cpl_render_ascii(gs.rs->ren,&aps->font_top,"SCORES:",GAME_GROUND_MARGIN+SCORES_TEXT_POS_X,SCORES_TEXT_POS_Y))
    SDL_Log("游戏页面渲染");
    map_clear(map);
    map_set(map,SDLK_ESCAPE,block_game_return_menu);
    map_set(map,SDLK_DOWN,block_game_down);
    map_set(map,SDLK_LEFT,block_game_left);
    map_set(map,SDLK_RIGHT,block_game_right);
    game_block_start();
    preview_update();
#if 1
    gs.timerId=SDL_AddTimer(bd.timerCount,blg_timer,NULL);
    if(gs.timerId==0){
        SDL_Log("SDL_AddTimer() error %s",SDL_GetError());
        return -1;
    }
#endif
    return 0;
}

int blg_action_dettach(void *pl)
{
    SDL_RemoveTimer(gs.timerId);
    gs.timerId=0;
    return 0;
}
int blg_action_event(SDL_Event *ev)
{
    switch(ev->user.code){
        case GS_GAMEERROR:
        SDL_Log("timer error: %s",(const char *)ev->user.data1);
        SDL_free(ev->user.data1);
        SDL_RemoveTimer(gs.timerId);
        gs.timerId=0;
        return -1;
        case GS_TIMERFALL:
        MINUS_ERR(block_game_down(NULL))
        break;        
    }
    return 0;
}
const STAGE_ACTION const_saction_block={
    blg_action_attach,
    blg_action_event,
    blg_action_dettach
};



int blockGameStageInit(RUNSTATE *rs,STAGE *stage){
    // 游戏区轮廓
    gs.blocksize=(rs->dm.h-MAIN_SCREEN_MARGIN_V-GAME_GROUND_MARGIN*2)/ROW_CNT;
    gs.gameLayer.x=300;
    gs.gameLayer.y=GAME_GROUND_MARGIN;
    // +1 是要预留一条边框位置
    gs.gameLayer.h=gs.blocksize * ROW_CNT+1;
    gs.gameLayer.w=gs.blocksize * COL_CNT+1;
    struct GRID_PARAM gp;

    gs.bgcolor.a=0xff;
    gs.bgcolor.r=0xaa;
    gs.bgcolor.g=0xbb;
    gs.bgcolor.b=0xcc;
    SDL_memcpy4(&gp.bgcolor,&gs.bgcolor,1);
    gp.gridsize=gs.blocksize;
    gs.blankBackground=cpl_create_texture_paint_pixels(rs->ren,gs.gameLayer.w,gs.gameLayer.h,grid_texture_callback,&gp);
    if(gs.blankBackground==NULL){
        return -1;
    }
    gs.previewLayer.x=300 + gs.gameLayer.w +50;
    gs.previewLayer.y=GAME_GROUND_MARGIN;
    gs.previewLayer.h=gs.blocksize * 4 +1;
    gs.previewLayer.w=gs.previewLayer.h;
    gs.blankNextPreview=cpl_create_texture_paint_pixels(rs->ren,gs.previewLayer.w,gs.previewLayer.h,grid_texture_callback,&gp);
    if(gs.blankNextPreview==NULL){
        return -1;
    }
    SDL_Log("方块:%d",gs.blocksize);
    // SDL_Rect rt_ground;
    stage->action=&const_saction_block;
    gs.rs=rs;
    stage->payload=&gs;
    bd.timerCount=700;
    gs.timerId=0;
    return 0;
}

