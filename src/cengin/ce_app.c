#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include "ce_app.h"
#define INIT_ACTION_SIZE 10

CE_APP* ce_init (int flags)
{
    const int us=(sizeof(EventAction));
    if (SDL_Init(flags))
    {
        SDL_LogError(0,"SDL init error %s",SDL_GetError());
        return NULL;
    }
    
    CE_APP *winren=malloc(sizeof(CE_APP));
    SDL_GetCurrentDisplayMode(0,&(winren->dm));
    winren->actions=malloc(us*INIT_ACTION_SIZE);
    winren->actionSize=INIT_ACTION_SIZE;
    winren->actionIdx=0;
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
    SDL_SetWindowTitle(win,title);
    return 0;
}
void ce_cleanup(CE_APP *app)
{
    SDL_DestroyRenderer(app->render);
    SDL_DestroyWindow(app->win);
    free(app->actions);
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

int ce_add_action(CE_APP *app,EventAction ea){
    const int us=sizeof(EventAction);
    EventAction *ptr=app->actions;
    int idx=app->actionIdx;
    ptr += idx;
    *ptr=ea;
    app->actionIdx =idx +1;
    if(app->actionIdx == app->actionSize){
        SDL_Log("event action array is realloc() because size enlarge");
        int size=app->actionSize;
        app->actions=realloc(app->actions,us * size *2);
        app->actionSize=size * 2;
    }
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
            for(int ix=0;ix<app->actionIdx;ix++){
                int res=(*(app->actions + ix))(app,&ev);
                if(res){
                    SDL_Log("return non-zero at event action");
                }
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