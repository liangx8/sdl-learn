#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct GRID_PARAM{
    // 底颜色
    SDL_Color bgcolor;
    int gridsize;
};
/**
 * @brief 画带格的背景图的回调函数
 * @param pixels 参考texture的修改回调
 * @param h
 * @param pitch
 * @param param 必须是struct GRID_PARAM*
 */
int grid_texture_callback(char *pixels,int h,int pitch,void *param)
{
    struct GRID_PARAM *gp=(struct GRID_PARAM *)param;
    SDL_Log("high:%d,pitch:%d\n",h,pitch);
    const int cols=pitch/4;
    for(int iy=0;iy<h;iy++){
        char *row=pixels+iy*pitch;
        for(int ix=0;ix<cols;ix++){
            char *ptr=row+ix*4;
            char lc=0;
            if(iy % gp->gridsize ==0){
                lc=20;
            }
            if(ix % gp->gridsize ==0){
                lc=20;
            }
            *ptr= 0xff;
            *(ptr+1)=gp->bgcolor.r - lc;
            *(ptr+2)=gp->bgcolor.g - lc;
            *(ptr+3)=gp->bgcolor.b - lc;
        }
    }
    return 0;
}


int rect_texture_callback(char *pixels,int h,int pitch,void *param)
{
    SDL_Color *color=(SDL_Color*)param;
    for(int iy=0;iy<h;iy++){
        // if(iy & 1) continue;
        for(int ix=0;ix<pitch;ix=ix+4)
        {
            char *ptr=pixels+iy*pitch+ix;
            *ptr=color->a;
            ptr++;
            *ptr=color->r;
            ptr++;
            *ptr=color->g;
            ptr++;
            *ptr=color->b;
        }
    }
    return 0;
}

/**
 * @brief 储存0x21到0x7e到字符
 */
struct TEXTURE_ASCII{
    SDL_Texture *texture;
    int w; // 每个字摸的宽度
    int h; // 高度
};
SDL_Texture* cpl_create_texture_paint_pixels(SDL_Renderer *ren,int w,int h,typeof(int (*)(char *,int,int,void *)) pt, void *param)
{
    // 回调函数中的pitch是y轴的字节数，因为每像素要用rgba来描述，所以要４字节
    SDL_Texture *ttr=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,w,h);
    SDL_Log("创建Texture(width:%d,height:%d)",w,h);
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
    for(int ix=0;ix<total;ix++){
        chs[ix]=0xff01+ix;
    }
    chs[total]='\0';
    SDL_Surface *surf=TTF_RenderUNICODE_Solid(font,&chs[0],color);
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
    SDL_Log("宽英文字体:%s, size(width:%d,height:%d)",fontname,tasc->w,tasc->h);
    return 0;
}
int cpl_create_texture_ascii(SDL_Renderer *ren,const char *fontname,SDL_Color color,int size,struct TEXTURE_ASCII *tasc)
{
    TTF_Font *font=TTF_OpenFont(fontname,size);
    if(font==NULL){
        SDL_Log("cpl_create_texture_ascii() open font error: %s",TTF_GetError());
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
        SDL_Log("cpl_create_texture_ascii() create surface error %s",TTF_GetError());
        return -1;
    }
    SDL_Texture *tx=SDL_CreateTextureFromSurface(ren,surf);
    SDL_FreeSurface(surf);
    if(tx==NULL){
        SDL_Log("cpl_create_texture_ascii() create texture error %s",SDL_GetError());
        return -1;
    }
    tasc->texture=tx;
    int w;
    if(SDL_QueryTexture(tx,NULL,NULL,&w,&tasc->h)){
        SDL_Log("cpl_create_texture_ascii() query texture error %s",SDL_GetError());
        return -1;
    }
    
    tasc->w=w/total;
    SDL_Log("英文字体:%s, size(width:%d,height:%d)",fontname,tasc->w,tasc->h);
    return 0;
}
SDL_Texture *cpl_create_texture_text(SDL_Renderer *ren,const char *fontname,const char *str,SDL_Color color,int size)
{
    TTF_Font *font=TTF_OpenFont(fontname,size);
    if(font==NULL){
        SDL_Log("cpl_create_texture_text() open font error: %s",TTF_GetError());
        return NULL;
    }
#if 1
    SDL_Surface *surf;
    //surf=TTF_RenderUTF8_Solid(font,str,color);
    surf=TTF_RenderUTF8_Blended(font,str,color);
#else
    SDL_Color bg={0xea,0xea,0xea,127};
    SDL_Surface *surf=TTF_RenderUTF8(font,str,color,bg);
#endif
    TTF_CloseFont(font);
    if(surf==NULL){
        SDL_Log("cpl_create_texture_text() create surface error %s",TTF_GetError());
        return NULL;
    }
    SDL_Texture *tx=SDL_CreateTextureFromSurface(ren,surf);
    SDL_FreeSurface(surf);
    if(tx==NULL){
        SDL_Log("cpl_create_texture_text() create texture error %s",SDL_GetError());
        return NULL;
    }
    return tx;
}
