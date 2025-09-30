#include "stage.h"
#include "cpl.h"
#include "map.h"
#include "abc_runstate.h"
#include "appstate.h"
#define MENU_FONT_WIDTH 48
const char *menustr="开始游戏选项按键定义玩腻了";
const int rng[]=    {        4,  6,      10,   13};
struct MENU_STATUS
{
    SDL_Texture *menustr;
    SDL_Rect *this;
    SDL_Rect *prev;
    SDL_Rect rects[4];
    int selected;
    int prev_sel;
    int fontw;
    int fonth;
} ms;


int keyup(RUNSTATE *rs,void * _)
{
    wprintf(L"向上\n");
    return 0;
}
int menu_start(RUNSTATE *rs,MAP map,void *obj)
{
    map_clear(map);
    if(map_set(map,SDLK_UP,keyup)){
        return -1;
    }
    if(map_set(map,SDLK_w,keyup)){
        return -1;
    }
    struct APPSTATE *aps=(struct APPSTATE*)obj;
    Uint32 color=aps->gs->bgcolor;
    Uint8 a= color & 0xff;
    Uint8 r=(color >> 8) & 0xff;
    Uint8 g=(color >> 16) & 0xff;
    Uint8 b=(color >> 24) & 0xff;
    //SDL_UnlockMutex(aps->mutex);
    SDL_SetRenderDrawColor(rs->ren,r,g,b,a);
    SDL_RenderClear(rs->ren);
    SDL_RenderCopy(rs->ren,aps->texture,NULL,&aps->textureRect);
    if(cpl_render_ascii(rs->ren,&aps->font_top,"3.14159",100,300)){
        SDL_Log("error:%s",SDL_GetError());
        return -1;
    }
    SDL_Rect src,dst;
    src.w=400;
    src.h=aps->font_top.h;
    src.x=0;
    src.y=0;
    dst.w=400;
    dst.h=src.h;
    dst.x=200;
    dst.y=400;
    SDL_RenderCopy(rs->ren,aps->font_top.texture,&src,&dst);

    dst.x=(rs->dm.w- (4 * ms.fontw)) /2;
    wprintf(L"screen w:%d most left: %d\n",rs->dm.w,dst.x);
    dst.h=ms.fonth;
    for(int ix=0;ix<4;ix++){
        dst.y=ix * (ms.fonth + 5)+50;
        dst.w=ms.rects[ix].w;
        SDL_RenderCopy(rs->ren,ms.menustr,&ms.rects[ix],&dst);
    }

    return 0;
}
int menu_run(RUNSTATE *rs,void *pl)
{
    return 0;
}
int menu_end(RUNSTATE *rs,void *pl)
{
    return 0;
}
const STAGE_ACTION const_stage_menu={
    menu_start,
    (action_func)menu_run,
    (action_func)menu_end
};

extern const char *const fontname[];
int stage_menu_init(RUNSTATE *rs,STAGE *ptr)
{
    ptr->action=&const_stage_menu;
    SDL_Color fg={0xf0,0xa8,0xe0,0xff};
    ms.menustr=cpl_create_texture_text(rs->ren,fontname[1],menustr,fg,MENU_FONT_WIDTH);
    if(ms.menustr==NULL){
        return -1;
    }
    SDL_QueryTexture(ms.menustr,NULL,NULL,&ms.fontw,&ms.fonth);
    ms.fontw=ms.fontw/13;
    SDL_Log("菜单字体:%s\n",fontname[1]);
    wprintf(L"menu texture width:%d,height:%d\n",ms.fontw,ms.fonth);
    if(ms.fontw!=MENU_FONT_WIDTH){
        SDL_Log("字体大小不匹配 expected %d,actual %d",MENU_FONT_WIDTH,ms.fontw);

    }
    int x=0;
    for(int ix=0;ix<4;ix++){
        ms.rects[ix].x=x;
        ms.rects[ix].y=0;
        ms.rects[ix].h=ms.fonth;
        ms.rects[ix].w=rng[ix]*ms.fontw-x;
        x=rng[ix]*ms.fontw+1;
    }
    ms.this=NULL;
    ms.prev=NULL;
    return 0;
}