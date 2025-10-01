#include "abc_stage.h"
#include "cpl.h"
#include "map.h"
#include "abc_runstate.h"
#define MENU_FONT_WIDTH 48
const char *menustr="开始游戏选项按键定义玩腻了";
const int rng[]=    {       4, 6,    10,  13};
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
    SDL_Rect dst;
    dst.x=(rs->dm.w-200- (4 * ms.fontw)) /2;
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

extern const char *const cjkfont[];
int stage_menu_init(RUNSTATE *rs,STAGE *ptr)
{
    ptr->action=&const_stage_menu;
    SDL_Color fg={0xf0,0xa8,0xe0,0xff};
    ms.menustr=cpl_create_texture_text(rs->ren,cjkfont[1],menustr,fg,MENU_FONT_WIDTH);
    if(ms.menustr==NULL){
        return -1;
    }
    SDL_QueryTexture(ms.menustr,NULL,NULL,&ms.fontw,&ms.fonth);
    ms.fontw=ms.fontw/13;
    SDL_Log("菜单字体:%s\n",cjkfont[1]);
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