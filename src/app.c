// https://examples.libsdl.org/SDL3/renderer/01-clear/


#include <stdio.h>
#include <malloc.h>

//#include <SDL3/SDL.h>
#include "appstate.h"
#include <SDL2/SDL_ttf.h>

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
const char *const fontname[];
int appevent(struct APPSTATE *,SDL_Event *);
int app_run(struct APPSTATE *,Uint32);
int updateBackground(void*);
struct GAMESTATE *newGameState(int row,int col);

#define BOARDER_WIDTH 25
int initAppState(struct APPSTATE *ptr)
{
    ptr->colors[0]=0xff;
    for(int ix=1;ix<256;ix++){
        ptr->colors[ix]=rand() | 0xff000000;
    }
    if(SDL_GetCurrentDisplayMode(0,&ptr->dm)){
        SDL_Log("%s",SDL_GetError());
        return -1;
    }
    ptr->gs=newGameState(40,20);
    const int wi=ptr->dm.w-400;
    const int hi=ptr->dm.h-200;
    if(SDL_CreateWindowAndRenderer(wi,hi,0,&ptr->win,&ptr->render)){
        SDL_Log("create window error %s",SDL_GetError());
        return -1;
    }
    ptr->textureRect.x=BOARDER_WIDTH;
    ptr->textureRect.y=BOARDER_WIDTH;
    ptr->textureRect.w=wi-BOARDER_WIDTH * 2;
    ptr->textureRect.h=hi-BOARDER_WIDTH * 2;
    SDL_Texture *texture=cpl_create_texture_paint_pixels(ptr->render,ptr->textureRect.w,ptr->textureRect.h,pt,NULL);
    if(texture==NULL){
        SDL_Log("texture error (%s)",SDL_GetError());
        return -1;
    }
    ptr->texture=texture;
    //cpl_create_texture_ascii_ucs2(ptr->render,fontname[0],red,30,&ptr->font_top);
    cpl_create_texture_ascii(ptr->render,fontname[0],red,30,&ptr->font_top);
    ptr->cpunum=SDL_GetCPUCount();
    SDL_Log("window size(h:%d,w:%d) CPU cores: \033[0;37;42m%d\033[0m\n",ptr->dm.h,ptr->dm.w,ptr->cpunum);
    ptr->runing=1;
    SDL_SetWindowTitle(ptr->win,"加油努力");
    ptr->mutex=SDL_CreateMutex();
    return 0;
}
void releaseAppState(struct APPSTATE *ptr)
{
    SDL_DestroyRenderer(ptr->render);
    SDL_DestroyWindow(ptr->win);
    SDL_DestroyTexture(ptr->texture);
    SDL_DestroyTexture(ptr->font_top.texture);
    SDL_DestroyMutex(ptr->mutex);
    free(ptr->gs);
}
const int fps=1000/24;


int main(int argc,char *argv[])
{
    struct APPSTATE aps;
    if(SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("initial error %s",SDL_GetError());
        return -1;
    }
    if(TTF_Init()){
        SDL_Log("init ttf error (%s)",TTF_GetError());
        return -1;
    }
    if(initAppState(&aps)){
        return -1;
    }
    Uint32 tick_prev=SDL_GetTicks();
    SDL_Thread *x= SDL_CreateThread(updateBackground,"background",&aps);
    int cnt=0;
    while(aps.runing){
        SDL_Event ev;
        while(SDL_PollEvent(&ev)){
            if(ev.type==SDL_QUIT){
                aps.runing=0;
            }
            if(appevent(&aps,&ev)){
                SDL_Log("some error %s",SDL_GetError());
                break;
            }
        }
        Uint32 tick_now=SDL_GetTicks();
        Uint32 peri=tick_now-tick_prev;
        if(peri > fps){
            tick_prev=tick_now;
            if(app_run(&aps,tick_now)){
                SDL_Log("some error %s",SDL_GetError());
                break;
            }
            if(aps.screenOut){
                aps.screenOut=0;
                SDL_RenderPresent(aps.render);
            }
            cnt++;
        }
    }
    SDL_WaitThread(x,NULL);
    releaseAppState(&aps);
    TTF_Quit();
    SDL_Quit();
    return 0;
}



const char *const fontname[]={
"/usr/share/fonts/SpaceMono-Regular.ttf",
"/usr/share/fonts/gnu-free/FreeMono.otf",
"/usr/share/fonts/noto-cjk/NotoSerifCJK-ExtraLight.ttc",
"/usr/share/fonts/SpaceMono-Bold.ttf",
"/usr/share/fonts/SpaceMono-BoldItalic.ttf",
"/usr/share/fonts/SpaceMono-Italic.ttf",
"/usr/share/fonts/德彪钢笔行书字库(3月9日更新).ttf",
"/usr/share/imlib2/data/fonts/notepad.ttf",
"/usr/share/fonts/noto/NotoColorEmoji.ttf",
"/usr/share/fonts/汉堡包手机字体.ttf",
"/usr/share/fonts/gnu-free/FreeMonoOblique.otf",
"/usr/share/fonts/落落-卿本佳人.ttf",
"/usr/share/fonts/gnu-free/FreeMonoBold.otf",
"/usr/share/fonts/gnu-free/FreeMonoBoldOblique.otf",
"/usr/share/feh/fonts/yudit.ttf",
"/usr/share/ppsspp/assets/Roboto-Condensed.ttf",
"/usr/share/vlc/skins2/fonts/FreeSansBold.ttf",
"/usr/share/vlc/skins2/fonts/FreeSans.ttf",
"/usr/share/fonts/noto-cjk/NotoSansCJK-Light.ttc",
"/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc",
"/usr/share/fonts/noto-cjk/NotoSerifCJK-Bold.ttc",
"/usr/share/fonts/noto-cjk/NotoSansCJK-Bold.ttc",
"/usr/share/fonts/noto-cjk/NotoSerifCJK-Regular.ttc",
"/usr/share/fonts/noto-cjk/NotoSerifCJK-Black.ttc",
"/usr/share/fonts/noto-cjk/NotoSansCJK-DemiLight.ttc",
"/usr/share/fonts/noto-cjk/NotoSansCJK-Thin.ttc",
"/usr/share/fonts/noto-cjk/NotoSerifCJK-Light.ttc",
"/usr/share/fonts/noto-cjk/NotoSerifCJK-Medium.ttc",
"/usr/share/fonts/noto-cjk/NotoSerifCJK-SemiBold.ttc",
"/usr/share/fonts/noto-cjk/NotoSansCJK-Medium.ttc",
"/usr/share/fonts/noto-cjk/NotoSansCJK-Black.ttc",
"/usr/share/imlib2/data/fonts/cinema.ttf",
"/usr/share/fonts/cantarell/Cantarell-VF.otf",
"/usr/share/imlib2/data/fonts/morpheus.ttf"
};
/*
// 每秒60帧
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
*/