// https://examples.libsdl.org/SDL3/renderer/01-clear/

#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_main.h>
#include "appstate.h"
#include "abc_runstate.h"

#define BG_BORDER_R 0x23
#define BG_BORDER_G 0xbf
#define BG_BORDER_B 0xa9

#define BG_R 0xfa
#define BG_G 0xeb
#define BG_B 0xd1
#define GRID_SIZE 80


SDL_Color red={0xff,0,0,0xff};

int pt(char *pixels,int h,int pitch,void *param)
{
    SDL_Log("high:%d,pitch:%d\n",h,pitch);
    int col[]={0,h-1};
    for(int cx=0;cx<2;cx++){
        for(int ix=0;ix<pitch;ix++){
            char *ptr1=pixels+col[cx]*pitch+ix;
            switch(ix & 3){
                case 0: // alpha
                    *ptr1=0xff; break;
                case 1: // red
                    *ptr1=BG_BORDER_R; break;
                case 2: // green
                    *ptr1=BG_BORDER_G; break;
                case 3: // blue
                    *ptr1=BG_BORDER_B; break;
            }
        }
    }
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
        for(int ix=4;ix<pitch-4;ix++){
            char *ptr=row+ix;
            char lc=0;
            if(iy % GRID_SIZE ==0){
                lc=20;
            }
            int tmp=ix /4;
            if(tmp % GRID_SIZE ==0){
                lc=20;
            }
            switch(ix & 3){
                case 0: // alpha
                    *ptr=0xff; break;
                case 1: // red
                    *ptr=BG_R - lc; break;
                case 2: // green
                    *ptr=BG_G - lc; break;
                case 3: // blue
                    *ptr=BG_B - lc; break;
            }
        }
    }
    return 0;
}
extern const char *const fontname[];
extern const char *const monofont[];
int appevent(struct APPSTATE *,SDL_Event *);
int app_run(struct APPSTATE *,Uint32);
int updateBackground(void*);
struct GAMESTATE *newGameState(int row,int col);

#define HAN_CHAR 1
#define BOARDER_WIDTH 25
int initAppState(struct APPSTATE *ptr,RUNSTATE *rs)
{
    ptr->colors[0]=0xff;
    for(int ix=1;ix<256;ix++){
        ptr->colors[ix]=rand() | 0xff000000;
    }
    const int wi=rs->dm.w-200;
    const int hi=rs->dm.h-100;
    ptr->gs=newGameState(40,20);
    ptr->textureRect.x=BOARDER_WIDTH;
    ptr->textureRect.y=BOARDER_WIDTH;
    ptr->textureRect.w=wi-BOARDER_WIDTH * 2;
    ptr->textureRect.h=hi-BOARDER_WIDTH * 2;
    SDL_Texture *texture=cpl_create_texture_paint_pixels(rs->ren,ptr->textureRect.w,ptr->textureRect.h,pt,NULL);
    if(texture==NULL){
        SDL_Log("texture error (%s)",SDL_GetError());
        return -1;
    }
    ptr->texture=texture;
#ifdef HAN_CHAR
    if(cpl_create_texture_ascii_ucs2(rs->ren,fontname[0],red,19,&ptr->font_top)){
#else
    if(cpl_create_texture_ascii(rs->ren,monofont[0],red,13,&ptr->font_top)){
#endif
        SDL_Log("texture error (%s)",SDL_GetError());
        return -1;
    }
    ptr->mutex=SDL_CreateMutex();
    ptr->rs=rs;
    return 0;
}
int initRunState(RUNSTATE *rs){
    if(SDL_GetCurrentDisplayMode(0,&rs->dm)){
        SDL_Log("%s",SDL_GetError());
        return -1;
    }
    const int wi=rs->dm.w-200;
    const int hi=rs->dm.h-100;
    if(SDL_CreateWindowAndRenderer(wi,hi,0,&rs->win,&rs->ren)){
        SDL_Log("create window error %s",SDL_GetError());
        return -1;
    }
    rs->cpunum=SDL_GetCPUCount();
    SDL_Log("window size(h:%d,w:%d) CPU cores: \033[0;37;42m%d\033[0m\n",rs->dm.h,rs->dm.w,rs->cpunum);
    rs->runing=1;
    SDL_SetWindowTitle(rs->win,"加油努力");
    rs->screenUpdate=1;
    return 0;
}
void releaseAppState(struct APPSTATE *ptr)
{
    SDL_DestroyTexture(ptr->texture);
    SDL_DestroyTexture(ptr->font_top.texture);
    SDL_DestroyMutex(ptr->mutex);
    free(ptr->gs);
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
const int fps=1000/24;


int app(void)
{
    struct APPSTATE aps;
    RUNSTATE rs;
    const Uint32 switchStageType = SDL_RegisterEvents(10);
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

    Uint32 tick_prev=SDL_GetTicks();
    SDL_Thread *x= SDL_CreateThread(updateBackground,"background",&aps);
    STAGE menu;
    STAGE *current=&menu;
    MAP keymap;
    keymap=map_new(64);
    stage_menu_init(&rs,&menu);
    menu.payload=&aps;
    menu.action->attach(&rs,keymap,menu.payload);
    
    while(rs.runing){
        SDL_Event ev;
        while(SDL_PollEvent(&ev)){
            if(ev.type==SDL_QUIT){
                rs.runing=0;
                break;
            }
            if(ev.type == SDL_KEYUP){
                int (*action)(void *);
                if(map_get(keymap,ev.key.keysym.sym,(void **)&action)){
                    break;
                }
                action(current->payload);
            }
            if(ev.type == switchStageType){
                STAGE *next=(STAGE *)ev.user.data1;
                current->action->dettech(&rs,current->payload);
                next->action->attach(&rs,keymap,next->payload);
                current=next;
            }
        }
        Uint32 tick_now=SDL_GetTicks();
        Uint32 peri=tick_now-tick_prev;

        if(current->action->run(&rs,current->payload)){
            SDL_Log("some error %s",SDL_GetError());
            break;
        }
        if(peri > fps){
            tick_prev=tick_now;
            if(rs.screenUpdate){
                rs.screenUpdate=0;
                wprintf(L"renderer check point\n");
                SDL_RenderPresent(rs.ren);
            }
        }
    }
    SDL_WaitThread(x,NULL);
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
