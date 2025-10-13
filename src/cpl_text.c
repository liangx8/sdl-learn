#include "cpl.h"
int cpl_render_ascii(SDL_Renderer *ren,struct TEXTURE_ASCII *tasc,const char *text,int x,int y)
{
    SDL_Rect src,dst;
    const char base=0x21;
    dst.h=tasc->h;
    src.h=tasc->h;
    dst.w=tasc->w;
    src.w=tasc->w;
    dst.x=x;
    dst.y=y;
    src.y=0;
    const char *ptr=text;
    while(*ptr){
        int idx=*ptr-base;
        src.x=idx * tasc->w;
        if(SDL_RenderCopy(ren,tasc->texture,&src,&dst)){
            SDL_Log("cpl_render_ascii() error %s",SDL_GetError());
            return -1;
        }
        dst.x += tasc->w;
        ptr++;
    }
    return 0;
}