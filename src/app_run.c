#include "appstate.h"


int updateBackground(struct APPSTATE *aps)
{
    Uint8 num=0;
    while(aps->runing){
        SDL_Delay(20);
        num++;
        //SDL_LockMutex(aps->mutex);
        aps->gs->bgcolor=0xff+(num << 8) + ((0xff - num) << 16) + ((num + 0x80)<<24);
        //SDL_UnlockMutex(aps->mutex);
        aps->screenOut=1;
    }
    return 0;
}
int app_run(struct APPSTATE *aps,Uint32 tick)
{
    if(aps->screenOut){
        //SDL_LockMutex(aps->mutex);
        Uint32 color=aps->gs->bgcolor;
        Uint8 a= color & 0xff;
        Uint8 r=(color >> 8) & 0xff;
        Uint8 g=(color >> 16) & 0xff;
        Uint8 b=(color >> 24) & 0xff;
        //SDL_UnlockMutex(aps->mutex);
        SDL_SetRenderDrawColor(aps->render,r,g,b,a);
        SDL_RenderClear(aps->render);
        SDL_RenderCopy(aps->render,aps->texture,NULL,&aps->textureRect);
        if(cpl_render_ascii(aps->render,&aps->font_top,"3.14159",200,200)){
            SDL_Log("error:%s",SDL_GetError());
            return -1;
        }
        SDL_Rect src,dst;
        src.w=200;
        src.h=aps->font_top.h;
        src.x=0;
        src.y=0;
        dst.w=200;
        dst.h=src.h;
        dst.x=200;
        dst.y=300;
        SDL_RenderCopy(aps->render,aps->font_top.texture,&src,&dst);
    }
    return 0;
}