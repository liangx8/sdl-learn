#include "abc.h"
#include "cpl.h"
#include "map.h"
#include "appres.h"
#define MENU_FONT_WIDTH 48
#define MENUSTR_LEN 16
const char *menustr="开始游戏选项按键定义玩腻了あいお";
const int rng[]=    {       4, 6,    10,  13,   MENUSTR_LEN};
#define MENUCNT     5
struct MENU_STATUS{
    RUNSTATE *rs;
    SDL_Texture *menustr;
    SDL_Texture *menuMask;
    SDL_Rect *this;
    SDL_Rect *prev;
    SDL_Rect rects[MENUCNT];
    int selected;
    int prev_sel;
    int fontw;
    int fonth;
    int maskw;
    int maskh;
} ms;

#define MENU_ROW_GAP 20
#define MENU_TOP_MARGIN 50
int menu_focus(void)
{
    APPRES *aps=(APPRES *)ms.rs->payload;
    SDL_Rect dst;
    dst.w=ms.maskw;
    dst.h=ms.maskh;

    dst.y=ms.selected * (ms.fonth + MENU_ROW_GAP) + MENU_TOP_MARGIN;
    dst.x=(ms.rs->winw- (4 * ms.fontw)) /2;
    SDL_RenderCopy(ms.rs->ren,ms.menuMask,NULL,&dst);
    if(ms.selected != ms.prev_sel){
        // 删除旧的mask
        dst.y=ms.prev_sel * (ms.fonth + MENU_ROW_GAP)+MENU_TOP_MARGIN;
        SDL_Rect src={dst.x-5,dst.y-5,dst.w,dst.h};
        MINUS_ERR(SDL_RenderCopy(ms.rs->ren,aps->textureBg,&src,&dst))
        dst.w=ms.rects[ms.prev_sel].w;
        MINUS_ERR(SDL_RenderCopy(ms.rs->ren,ms.menustr,&ms.rects[ms.prev_sel],&dst))
        ms.prev_sel=ms.selected;
    }
    ms.rs->present=1;
    // SDL_Event ev;
    // ev.type=SDL_WINDOWEVENT;
    // ev.window.event=SDL_WINDOWEVENT_EXPOSED;
    // SDL_PushEvent(&ev);
    return 0;
}
int menu_down(void *_)
{
    if(ms.selected<MENUCNT-1){
        ms.selected++;
    } else {
        ms.selected=0;
    }
    return menu_focus();
}
int keyup(void * pl)
{
    if(ms.selected){
        ms.selected--;
    } else {
        ms.selected=MENUCNT-1;
    }
    return menu_focus();
}
int menu_return(void *pl)
{
    switch(ms.selected){
        case 0:
#if 1
        APPRES *aps=(APPRES *)ms.rs->payload;
        MINUS_ERR(switch_stage(aps->game));
#endif
        break;
        case 3:
        SDL_Event ev;
        ev.type=SDL_QUIT;
        SDL_PushEvent(&ev);
        break;
    }

    return 0;
}
int menu_esc(void *pl)
{
    SDL_Event ev;
    ev.type=SDL_QUIT;
    SDL_PushEvent(&ev);
    return 0;
}
int menu_start(MAP map)
{
    RUNSTATE *rs=ms.rs;
    APPRES *aps=(APPRES*)rs->payload;
        //画背景
    MINUS_ERR(SDL_RenderCopy(rs->ren,aps->textureBg,NULL,&aps->rectBg))
    map_clear(map);
    if(map_set(map,SDLK_UP,keyup)){
        return -1;
    }
    if(map_set(map,SDLK_w,keyup)){
        return -1;
    }
    if(map_set(map,SDLK_RETURN,menu_return)){
        return -1;
    }
    if(map_set(map,SDLK_ESCAPE,menu_esc)){
        return -1;
    }
    if(map_set(map,SDLK_DOWN,menu_down)){
        return -1;
    }
    SDL_Rect dst;
    dst.x=(rs->winw- (4 * ms.fontw)) /2;
    wprintf(L"显示菜单 screen w:%d most left: %d\n",rs->dm.w,dst.x);
    dst.h=ms.fonth;
    for(int ix=0;ix<MENUCNT;ix++){
        dst.y=ix * (ms.fonth + MENU_ROW_GAP)+MENU_TOP_MARGIN;
        dst.w=ms.rects[ix].w;
#if 0
        if(ix==0){
            //这个方法只能改变背景色，而且还会把前景色搞没了
            if(SDL_SetTextureColorMod(ms.menustr,0xff,0,0)){
                SDL_Log("SetTextureColorMod() error1 with %s",SDL_GetError());
            }
        } else {
            if(SDL_SetTextureColorMod(ms.menustr,0,0,0xff)){
                SDL_Log("SetTextureColorMod() error2 with %s",SDL_GetError());
            }
        }
#endif
        MINUS_ERR(SDL_RenderCopy(rs->ren,ms.menustr,&ms.rects[ix],&dst))
    }
    ms.selected=0;
    ms.prev_sel=0;
    menu_focus();
    return 0;
}
int menu_run(SDL_Event *pl)
{
    return 0;
}
int menu_end(void *pl)
{
    return 0;
}
const STAGE_ACTION const_stage_menu={
    (action_func)menu_start,
    menu_run,
    (action_func)menu_end
};

extern const char *const cjkfont[];
int stage_menu_init(RUNSTATE *rs,STAGE *ptr)
{
    const char *usefont=cjkfont[0];
    ptr->action=&const_stage_menu;
    //0838be
    SDL_Color fg={0x08,0x38,0xbe,0xff};
    ms.menustr=cpl_create_texture_text(rs->ren,usefont,menustr,fg,MENU_FONT_WIDTH);
    if(ms.menustr==NULL){
        return -1;
    }
    SDL_QueryTexture(ms.menustr,NULL,NULL,&ms.fontw,&ms.fonth);
    ms.fontw=ms.fontw/MENUSTR_LEN;
    SDL_Log("菜单字体:%s\n",usefont);
    wprintf(L"menu texture width:%d,height:%d\n",ms.fontw,ms.fonth);
    if(ms.fontw!=MENU_FONT_WIDTH){
        SDL_Log("字体大小不匹配 expected %d,actual %d",MENU_FONT_WIDTH,ms.fontw);
        
    }
    //bd53f1
    fg.a=80;
    fg.r=23;
    fg.g=23;
    fg.b=230;
    ms.menuMask=cpl_create_texture_paint_pixels(
        rs->ren,
        200,
        ms.fonth,
        rect_texture_callback,
        &fg);
    if(ms.menuMask==NULL){
        return -1;
    }
    ms.maskh=ms.fonth;
    ms.maskw=200;
    MINUS_ERR(SDL_SetTextureBlendMode(ms.menuMask,SDL_BLENDMODE_BLEND))
    MINUS_ERR(SDL_SetTextureAlphaMod(ms.menuMask,127))
    // SDL_SetTextureColorMod(ms.menuMask,0xf1,0x53,0xbd);
    int x=0;
    for(int ix=0;ix<MENUCNT;ix++){
        ms.rects[ix].x=x;
        ms.rects[ix].y=0;
        ms.rects[ix].h=ms.fonth;
        ms.rects[ix].w=rng[ix]*ms.fontw-x;
        x=rng[ix]*ms.fontw+1;
    }
    ms.this=NULL;
    ms.prev=NULL;
    ms.rs=rs;
    ptr->payload=rs;
    return 0;
}
