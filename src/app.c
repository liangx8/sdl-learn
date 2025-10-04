
#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <SDL2/SDL_ttf.h>
#include "appres.h"
#include "abc_stage.h"

#define BG_BORDER_R 0x23
#define BG_BORDER_G 0xbf
#define BG_BORDER_B 0xa9

#define BG_R 0xfa
#define BG_G 0xeb
#define BG_B 0xd1
#define GRID_SIZE 40


SDL_Color red={0xff,0,0,0xff};
#if 0
int pt(char *pixels,int h,int pitch,void *param)
{
    
    SDL_Log("high:%d,pitch:%d\n",h,pitch);
    const int cols=pitch/4;
    int col[]={0,h-1};
    // ２水平边
    for(int cx=0;cx<2;cx++){
        for(int ix=0;ix<cols;ix++){
            char *ptr1=pixels+col[cx]*pitch+ix*4;
            *ptr1=0xff;
            ptr1++;
            *ptr1=BG_BORDER_R;
            ptr1++;
            *ptr1=BG_BORDER_G;
            ptr1++;
            *ptr1=BG_BORDER_B;
        }
    }
    // 画２垂直边
    for(int rx=0;rx<2;rx++){
        int row[]={0,pitch-4};
        for(int iy=0;iy<h;iy++){
            char *ptr=pixels+iy*pitch+row[rx];
            *ptr= 0xff;
            *(ptr+1)=BG_BORDER_R;
            *(ptr+2)=BG_BORDER_G;
            *(ptr+3)=BG_BORDER_B;
        }
    }
    for(int iy=1;iy<h-1;iy++){
        char *row=pixels+iy*pitch;
        for(int ix=0;ix<cols-1;ix++){
            char *ptr=row+ix*4;
            char lc=0;
            if(iy % GRID_SIZE ==0){
                lc=20;
            }
            if(ix % GRID_SIZE ==0){
                lc=20;
            }
            *ptr= 0xff;
            *(ptr+1)=BG_R - lc;
            *(ptr+2)=BG_G - lc;
            *(ptr+3)=BG_B - lc;
        }
    }
    return 0;
}
#endif
extern const char *const fontname[];
extern const char *const monofont[];
extern const char *const cjkfont[];

//#define HAN_CHAR 1
#define BOARDER_SIZE 5
int initAppState(APPRES *ptr,RUNSTATE *rs)
{
    for(int ix=1;ix<256;ix++){
        ptr->colors[ix]=rand() | 0xff000000;
    }
    const int wi=rs->dm.w-MAIN_SCREEN_MARGIN_H;
    const int hi=rs->dm.h-MAIN_SCREEN_MARGIN_V;
    ptr->textureRect.x=BOARDER_SIZE;
    ptr->textureRect.y=BOARDER_SIZE;
    ptr->textureRect.w=wi-BOARDER_SIZE * 2;
    ptr->textureRect.h=hi-BOARDER_SIZE * 2;
    struct GRID_PARAM gp;

    gp.gridsize=GRID_SIZE;
    gp.bgcolor.a=0xff;
    gp.bgcolor.r=BG_R;
    gp.bgcolor.g=BG_G;
    gp.bgcolor.b=BG_B;

    SDL_Texture *texture=cpl_create_texture_paint_pixels(rs->ren,ptr->textureRect.w,ptr->textureRect.h,grid_texture_callback,&gp);
    if(texture==NULL){
        SDL_Log("texture error (%s)",SDL_GetError());
        return -1;
    }
    ptr->textureBg=texture;
#ifdef HAN_CHAR
    if(cpl_create_texture_ascii_ucs2(rs->ren,cjkfont[0],red,19,&ptr->font_top)){
#else
    if(cpl_create_texture_ascii(rs->ren,monofont[0],red,19,&ptr->font_top)){
#endif
        SDL_Log("texture error (%s)",SDL_GetError());
        return -1;
    }
    // ptr->mutex=SDL_CreateMutex();
    rs->payload=ptr;
    return 0;
}
int initRunState(RUNSTATE *rs){
    if(SDL_GetCurrentDisplayMode(0,&rs->dm)){
        SDL_Log("%s",SDL_GetError());
        return -1;
    }
    const int wi=rs->dm.w-MAIN_SCREEN_MARGIN_H;
    const int hi=rs->dm.h-MAIN_SCREEN_MARGIN_V;
    if(SDL_CreateWindowAndRenderer(wi,hi,0,&rs->win,&rs->ren)){
        SDL_Log("create window error %s",SDL_GetError());
        return -1;
    }
    rs->cpunum=SDL_GetCPUCount();
    SDL_Log("window size(h:%d,w:%d) CPU cores: \033[0;37;42m%d\033[0m\n",rs->dm.h,rs->dm.w,rs->cpunum);
    rs->runing=1;
    SDL_SetWindowTitle(rs->win,"加油努力");
    rs->switchStageType = SDL_RegisterEvents(10);
    return 0;
}
void releaseAppState(APPRES *ptr)
{
    SDL_DestroyTexture(ptr->textureBg);
    SDL_DestroyTexture(ptr->font_top.texture);
//    SDL_DestroyMutex(ptr->mutex);
}
int updateBackground(void *x)
{
    return 0;
}
/*
typedef struct SDL_KeyboardEvent
{
    Uint32 type;        **< SDL_KEYDOWN or SDL_KEYUP *
    Uint32 timestamp;   **< In milliseconds, populated using SDL_GetTicks() *
    Uint32 windowID;    **< The window with keyboard focus, if any *
    Uint8 state;        **< SDL_PRESSED or SDL_RELEASED *
    Uint8 repeat;       **< Non-zero if this is a key repeat *
    Uint8 padding2;
    Uint8 padding3;
    SDL_Keysym keysym;  **< The key that was pressed or released *
} SDL_KeyboardEvent;
*/

int stage_menu_init(RUNSTATE *,STAGE *);
int blockGameStageInit(RUNSTATE *rs,STAGE *);
const int fps=1000/24;
int app(void)
{
    APPRES aps;
    RUNSTATE rs;
    if(SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("initial error %s",SDL_GetError());
        return -1;
    }
    if(TTF_Init()){
        SDL_Log("init ttf error (%s)",TTF_GetError());
        return -1;
    }
    if(initRunState(&rs)){
        return -1;
    }
    if(initAppState(&aps,&rs)){
        return -1;
    }

    STAGE menu,game;
    STAGE *current=&menu;
    MAP keymap;
    keymap=map_new(64);
    stage_menu_init(&rs,&menu);
    blockGameStageInit(&rs,&game);
    aps.menu=&menu;
    aps.game=&game;
    menu.action->attach(keymap,NULL);
    SDL_Event ev;
    while(SDL_WaitEvent(&ev)){

        switch(ev.type){
            case SDL_QUIT:
            rs.runing=0;
            goto exit_loop;
            case SDL_KEYUP:
            {
                action_func action;
                if(map_get(keymap,ev.key.keysym.sym,(void **)&action)){
                    continue;
                }
                action(current->payload);
            }
            break;
            case SDL_WINDOWEVENT:
            if(ev.window.event==SDL_WINDOWEVENT_EXPOSED){
                SDL_RenderPresent(rs.ren);
                //SDL_Log("window event occur!event:%d,type:%d,data1:%d,data2:%d",ev.window.event,ev.window.type,ev.window.data1,ev.window.data2);
            }
            break;
            default:
            if(ev.type==rs.switchStageType)
            {
                STAGE *next=(STAGE *)ev.user.data1;
                current->action->dettech(current->payload);
                next->action->attach(keymap,ev.user.data2);
                current=next;
                SDL_RenderPresent(rs.ren);
            }
            break;
        }
    }
    exit_loop:
    //SDL_WaitThread(x,NULL);
    releaseAppState(&aps);
    TTF_Quit();
    SDL_Quit();
    map_free(keymap);
    return 0;
}
int main(int argc,char **argv)
{
    int opt;
    setlocale(LC_ALL,"");
    while((opt=getopt(argc,argv,"t:"))!=-1){
        switch(opt){
        case 't':
        //test_pcf("/usr/share/fonts/wenquanyi/wenquanyi_13px.pcf");
        return 0;
        case '?':
        wprintf(L"wrong arguments\n");
        return 0;
        }
    }
    return app();
}
