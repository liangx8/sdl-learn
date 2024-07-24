#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include "ce_app.h"
#define INIT_ACTION_SIZE 10

CE_APP* ce_init (int flags)
{
    if (SDL_Init(flags))
    {
        SDL_LogError(0,"SDL init error %s",SDL_GetError());
        return NULL;
    }
    int num;
    if((num=SDL_NumJoysticks())<1){
        SDL_LogWarn(0,"没发现手柄");
    } else {
        SDL_Log("检测到%d个手柄",num);
    }
    CE_APP *winren=malloc(sizeof(CE_APP));
    SDL_GetCurrentDisplayMode(0,&(winren->dm));
    return winren;

}
int ce_create_window (CE_APP *app,const char *title,int width,int height,int flags)
{
    SDL_Window *win;
    SDL_Renderer *ren;
    if(SDL_CreateWindowAndRenderer(width,height,flags,&win,&ren)){
        SDL_LogError(0,"Create window and renderer error %s",SDL_GetError());
        // 应用最开始，如果遇到错误后面无法执行，app的内存也没有意义，因此释放
        free(app);
        return -1;
    }
    app->win=win;
    app->render=ren;
    app->win_w=width;
    app->win_h=height;
    SDL_SetWindowTitle(win,title);
    return 0;
}
void ce_cleanup(CE_APP *app)
{
    SDL_DestroyRenderer(app->render);
    SDL_DestroyWindow(app->win);
    
    free(app);
}

int ce_blit_surface  (CE_APP *app,SDL_Surface *surf,SDL_Rect *src,SDL_Rect *dst)
{
    SDL_Surface *target=SDL_GetWindowSurface(app->win);
    SDL_Surface *tmp=SDL_ConvertSurface(surf,target->format,0);
    if(tmp==NULL){
        SDL_LogError(0,"Convert Surface error: %s",SDL_GetError());
        return -1;
    }
    if(SDL_BlitSurface(tmp,src,target,dst)){
        SDL_LogError(0,"Blit surface error");
        SDL_FreeSurface(tmp);
        SDL_FreeSurface(target);
        return -1;
    }
    if(SDL_UpdateWindowSurface(app->win)){
        SDL_LogError(0,"Update window surface error: %s",SDL_GetError());
        return -1;
    }
    SDL_FreeSurface(tmp);
    SDL_FreeSurface(target);
    
    return 0;
}


int ce_run(CE_APP *app)
{
    SDL_Event ev;
    while(1){
        while(SDL_PollEvent(&ev)){
            if(ev.type==SDL_QUIT){
                goto outer_break;
            }
            int res=app->on_event(app,&ev);
            if(res){
                goto outer_break;
            }
        }
        app->present(app);
        //SDL_RenderClear(app->render);
        //SDL_RenderPresent(app->render);
    }
outer_break:
    return 0;
}
int ce_render_copy(CE_APP *app,SDL_Texture *texture, int x,int y)
{
    SDL_Rect rect;
    if(SDL_QueryTexture(texture,NULL,NULL,&rect.w,&rect.h)){
        SDL_Log("query texture error %s",SDL_GetError());
        return -1;
    }
    rect.x=x;
    rect.y=y;
    if(SDL_RenderCopy(app->render,texture,NULL,&rect)){
        SDL_Log("RenderCopy error %s",SDL_GetError());
        return -1;
    }
    return 0;
}