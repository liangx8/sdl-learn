// https://examples.libsdl.org/SDL3/renderer/01-clear/


#include <stdio.h>
#include <malloc.h>
#define SDL_MAIN_USE_CALLBACKS 1
//#include <SDL3/SDL.h>
#include "cpl.h"
#include <SDL3/SDL_main.h>
#include "appstate.h"
#define AREA_W 1024
#define AREA_H 768

#define BG_BORDER_R 0x23
#define BG_BORDER_G 0xbf
#define BG_BORDER_B 0xa9

#define BG_R 0xfa
#define BG_G 0xeb
#define BG_B 0xd1
#define GRID_SIZE 80
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
SDL_AppResult SDL_AppInit(void **appstate,int argc,char *argvp[])
{
    SDL_SetAppMetadata("游戏开发3","0.1","identifier");
    if(!SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("intial error with %s",SDL_GetError());
        return SDL_APP_FAILURE;
    }
    const SDL_DisplayMode *dm=SDL_GetCurrentDisplayMode(1);
    if(dm==NULL){
        SDL_Log("%s",SDL_GetError());
    }
    struct APPSTATE *ptr=malloc(sizeof(struct APPSTATE));
    ptr->winsize.x=dm->w-400;
    ptr->winsize.y=dm->h-200;
    if(!SDL_CreateWindowAndRenderer("汉字显示",ptr->winsize.x,ptr->winsize.y,0,&ptr->win,&ptr->render)){
        SDL_Log("create window error %s",SDL_GetError());
        return SDL_APP_FAILURE;
    }
    *appstate=ptr;
    SDL_Texture *texture=cpl_create_texture_paint_pixels(ptr->render,AREA_W,AREA_H,pt,NULL);
    if(texture==NULL){
        return SDL_APP_FAILURE;
    }
    ptr->texture=texture;
    ptr->tick=SDL_GetTicks();
    ptr->cpunum=SDL_GetNumLogicalCPUCores();
    SDL_Log("window size(h:%d,w:%d) CPU cores: %d\n",dm->h,dm->w,ptr->cpunum);
    // if(TTF_Init()){
    //     SDL_LogError(0,"ttf initial error %s",TTF_GetError());
    //     return SDL_APP_FAILURE;
    // }

    return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppEvent(void *appstate,SDL_Event *event)
{
    if(event->type == SDL_EVENT_QUIT){
        SDL_Log("正常退出");
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}
// 每秒60帧
const int fps=1000/60;
SDL_AppResult SDL_AppIterate(void *appstate)
{
    struct APPSTATE *aps=appstate;
    Uint64 now= SDL_GetTicks();
    if(now - aps->tick >fps ){
        aps->tick=now;
        const uint64_t base=now / fps;
        SDL_SetRenderDrawColor(aps->render,base & 0xff,0xff - (base & 0xff),(base + 127) & 0xff,0xff);
        SDL_RenderClear(aps->render);
        SDL_FRect dst={(aps->winsize.x-AREA_W)/2,(aps->winsize.y-AREA_H)/2,AREA_W,AREA_H};
        SDL_RenderTexture(aps->render,aps->texture,NULL,&dst);
        //SDL_RenderTextureTiled(aps->render,aps->texture,NULL,1.,NULL);
        SDL_RenderPresent(aps->render);
    }
    return SDL_APP_CONTINUE;
}
void SDL_AppQuit(void *appstate,SDL_AppResult result)
{
    struct APPSTATE *aps=appstate;
    SDL_DestroyTexture(aps->texture);
    free(appstate);
}