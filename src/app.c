// https://examples.libsdl.org/SDL3/renderer/01-clear/


#include <stdio.h>
#include <malloc.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window *win;
SDL_Renderer *render;

SDL_AppResult SDL_AppInit(void **appstate,int argc,char *argvp[])
{
    SDL_SetAppMetadata("游戏开发3","0.1","identifier");
    if(!SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("intial error with %s",SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if(!SDL_CreateWindowAndRenderer("汉字显示",640,480,0,&win,&render)){
        SDL_Log("create window error %s",SDL_GetError());
        return SDL_APP_FAILURE;
    }
    uint64_t *ptr=malloc(sizeof(long));
    *appstate=ptr;
    *ptr=SDL_GetTicks();
    
    return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppEvent(void *appstate,SDL_Event *event)
{
    if(event->type == SDL_EVENT_QUIT){
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}
// 每秒60帧
const int fps=1000/60;
SDL_AppResult SDL_AppIterate(void *appstate)
{
    Uint64 tick=SDL_GetTicks();
    Uint64 *ptr=appstate;
    
    if(tick - *ptr>fps ){
        *ptr=tick;
        const uint64_t base=tick / fps;
        SDL_SetRenderDrawColor(render,base & 0xff,0xff - (base & 0xff),(base + 127) & 0xff,0xff);
        SDL_RenderClear(render);
        SDL_RenderPresent(render);
    }
    return SDL_APP_CONTINUE;
}
void SDL_AppQuit(void *appstate,SDL_AppResult result)
{
    free(appstate);
}