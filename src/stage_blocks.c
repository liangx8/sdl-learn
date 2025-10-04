#include <malloc.h>
#include <strings.h>
#include "abc_runstate.h"
#include "abc_stage.h"
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
    SDL_Rect     gameLayer;
    SDL_Rect     previewLayer;
    SDL_Color    bgcolor;
    int          blocksize;
} gs;
extern struct BLOCK_DATA bd;
int gen_next(void);
int preview_update(void)
{
    gen_next();
    Uint16 type=bd.next;
    APPRES *aps=(APPRES*)gs.rs->payload;
    Uint32 color=aps->colors[bd.nextColorIdx];
    SDL_Rect rect;
    int basex,basey;
    basex=gs.previewLayer.x+1;
    basey=gs.previewLayer.y+1;
    rect.w=gs.blocksize-1;
    rect.h=gs.blocksize-1;
    for(int ix=0;ix<4;ix++){
        for(int iy=0;iy<4;iy++){
            if(type==0){
                goto outer_break;
            }
            rect.x=basex + iy * gs.blocksize;
            rect.y=basey;
            if(type & 1){
                SDL_SetRenderDrawColor(gs.rs->ren,color & 0xff,(color >> 8) & 0xff,(color >> 16) & 8,0xff);
                SDL_RenderFillRect(gs.rs->ren,&rect);
            }
            type = type >>1;
        }
        basey += gs.blocksize;

    }
    outer_break:
    return 0;
}
int block_game_attach(MAP map,void *pl)
{
    APPRES *aps=(APPRES *)gs.rs->payload;
    SDL_RenderCopy(gs.rs->ren,gs.blankBackground,NULL,&gs.gameLayer);
    SDL_RenderCopy(gs.rs->ren,gs.blankNextPreview,NULL,&gs.previewLayer);
    cpl_render_ascii(gs.rs->ren,&aps->font_top,"SCORES:",GAME_GROUND_MARGIN+20,100);
    SDL_Log("游戏页面渲染");
    map_clear(map);
    preview_update();
    // SDL_Event ev;
    // ev.type=SDL_WINDOWEVENT;
    // ev.window.event=SDL_WINDOWEVENT_EXPOSED;
    // SDL_PushEvent(&ev);    
    return 0;
}
int block_game_dettach(void *pl)
{
    return 0;
}
const STAGE_ACTION const_saction_block={
    block_game_attach,
    (action_func)NULL,
    block_game_dettach
};



int blockGameStageInit(RUNSTATE *rs,STAGE *stage){
    // 游戏区轮廓
    APPRES *aps=(APPRES *)rs->payload;
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
    return 0;
}

