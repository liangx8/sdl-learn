#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "ce_app.h"


SDL_Texture *ce_texture_create_from_image(CE_APP *app,const char *image)
{
    SDL_Texture *texture=IMG_LoadTexture(app->render,image);
    if(texture==NULL){
        SDL_LogError(0,"load texture from image error %s\n",IMG_GetError());
        return NULL;
    }
    return texture;
}
SDL_Texture *ce_texture_create_from_text(CE_APP *app,TTF_Font *font,const char* text,SDL_Color color)
{
    SDL_Surface *surf=TTF_RenderUTF8_Solid(font,text,color);
    if(surf == NULL){
        SDL_LogError(0,"create surface from text error %s\n",TTF_GetError());
        return NULL;
    }
    SDL_Texture *texture=SDL_CreateTextureFromSurface(app->render,surf);
    SDL_FreeSurface(surf);
    if(texture==NULL){
        SDL_LogError(0,"Create texture from surface error %s\n",SDL_GetError());
        return NULL;
    }
    return texture;
}
SDL_Texture *ce_texture_create_target(CE_APP *app,PaintTexture tt,int w,int h,void *param)
{
    SDL_Texture *ttr=SDL_CreateTexture(app->render,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,w,h);
    if(ttr==NULL){
        SDL_LogError(0,"create texture as rendere target error %s",SDL_GetError());
        return NULL;
    }
    SDL_SetRenderTarget(app->render,ttr);
    if(tt(app->render,w,h,param)){
        SDL_DestroyTexture(ttr);
        return NULL;
    }
    SDL_SetRenderTarget(app->render,NULL);
    return ttr;
}
int ce_texture_paint_pixels(SDL_Texture *texture,PaintPixels pt,void *param)
{

    void *pixels;
    int pitch;
    int access;
    int w,h;
    SDL_QueryTexture(texture,NULL,&access,&w,&h);
    // SDL_Rect 参数暂时不用，因为还搞不懂当有这个参数后，pixels的高度是怎么设定
    if(SDL_LockTexture(texture,NULL,&pixels,&pitch)){
        SDL_LogError(0,"Lock texture error %s",SDL_GetError());
        return -1;
    }
    SDL_Log("TEXTURE(%d,%d),pitch:%d",w,h,pitch);
    pt(pixels,pitch,h,param);
    SDL_UnlockTexture(texture);
    return 0;
}