#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


/**
 * @brief 储存0x21到0x7e到字符
 */
struct TEXTURE_ASCII{
    SDL_Texture *texture;
    int w; // 每个字摸的宽度
    int h; // 高度
};

SDL_Texture* cpl_create_texture_paint_pixels(SDL_Renderer *ren,int w,int h,int (*pt)(char *,int,int,void *), void *param)
{
    SDL_Texture *ttr=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,w,h);
    char *pixels;
    int pitch;
    if(ttr==NULL){
        return NULL;
    }
    if(SDL_LockTexture(ttr,NULL,(void **)&pixels,&pitch)){
        SDL_Log("lock texture error %s",SDL_GetError());
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
int cpl_create_texture_ascii_ucs2(SDL_Renderer *ren,const char *fontname,SDL_Color color,int size,struct TEXTURE_ASCII *tasc)
{
    TTF_Font *font=TTF_OpenFont(fontname,size);
    if(font==NULL){
        return -1;
    }
    // 0xff01 - 0xff5f
    const int total=0xff5f-0xff01;
    Uint16 chs[total+1];
    for(int ix=0x21;ix<127;ix++){
        chs[ix-0x21]=ix;
    }
    chs[total]='\0';
    SDL_Color bg={0,0,0,0};
    SDL_Surface *surf=TTF_RenderUNICODE(font,&chs[0],color,bg);
    TTF_CloseFont(font);
    if(surf==NULL){
        return -1;
    }
    SDL_Texture *tx=SDL_CreateTextureFromSurface(ren,surf);
    SDL_FreeSurface(surf);
    if(tx==NULL){
        TTF_CloseFont(font);
        return -1;
    }
    tasc->texture=tx;
    int w;
    SDL_QueryTexture(tx,NULL,NULL,&w,&tasc->h);
    tasc->w=w/total;
    SDL_Log("font:%s, size(width:%d,height:%d)",fontname,tasc->w,tasc->h);
    return 0;
}
int cpl_create_texture_ascii(SDL_Renderer *ren,const char *fontname,SDL_Color color,int size,struct TEXTURE_ASCII *tasc)
{
    TTF_Font *font=TTF_OpenFont(fontname,size);
    if(font==NULL){
        return -1;
    }
    // 0xff01 - 0xff5f
    const int total=0x7f-0x21;
    char chs[total+1];
    for(int ix=0x21;ix<127;ix++){
        chs[ix-0x21]=ix;
    }
    chs[total]='\0';
    SDL_Log("[\033[0;31;47m%s\033[0m]",&chs[0]);
    SDL_Surface *surf=TTF_RenderUTF8_Solid(font,&chs[0],color);
    TTF_CloseFont(font);
    if(surf==NULL){
        return -1;
    }
    SDL_Texture *tx=SDL_CreateTextureFromSurface(ren,surf);
    SDL_FreeSurface(surf);
    if(tx==NULL){
        return -1;
    }
    tasc->texture=tx;
    int w;
    SDL_QueryTexture(tx,NULL,NULL,&w,&tasc->h);
    tasc->w=w/total;
    SDL_Log("font:%s, size(width:%d,height:%d)",fontname,tasc->w,tasc->h);
    return 0;




}