#include <SDL3/SDL.h>

SDL_Texture* cpl_create_texture_paint_pixels(SDL_Renderer *ren,int w,int h,int (*pt)(char *,int,int,void *), void *param)
{
    SDL_Texture *ttr=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,w,h);
    char *pixels;
    int pitch;
    if(ttr==NULL){
        return NULL;
    }
    if(!SDL_LockTexture(ttr,NULL,(void **)&pixels,&pitch)){
        SDL_DestroyTexture(ttr);
        return NULL;
    }
    if(pt(pixels,h,pitch,param)){
        SDL_UnlockTexture(ttr);
        SDL_DestroyTexture(ttr);
        return NULL;
    }
    SDL_UnlockTexture(ttr);
    return ttr;
}
