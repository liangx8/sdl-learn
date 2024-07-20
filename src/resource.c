#include <SDL2/SDL_ttf.h>
#include "cengin/ce_app.h"
#include "resource.h"

const char *const fontname[]={
"/usr/share/fonts/gnu-free/FreeMono.otf",
"/usr/share/fonts/gnu-free/FreeMonoOblique.otf",
"/usr/share/fonts/gnu-free/FreeMonoBold.otf",
"/usr/share/fonts/gnu-free/FreeMonoBoldOblique.otf",
"/usr/share/feh/fonts/yudit.ttf",
"/usr/share/ppsspp/assets/Roboto-Condensed.ttf",
"/usr/share/vlc/skins2/fonts/FreeSansBold.ttf",
"/usr/share/vlc/skins2/fonts/FreeSans.ttf",
"/usr/share/fonts/noto-cjk/NotoSansCJK-Light.ttc",
"/usr/share/fonts/noto-cjk/NotoSerifCJK-ExtraLight.ttc",
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
"/usr/share/imlib2/data/fonts/notepad.ttf",
"/usr/share/imlib2/data/fonts/cinema.ttf",
"/usr/share/imlib2/data/fonts/morpheus.ttf"
};


int paint_background(SDL_Renderer *render,int w,int h,void * unused)
{
    SDL_SetRenderDrawColor(render,0,0,255,255);
    //SDL_RenderDrawLine(render,0,0,0,h);
    SDL_Rect re={0,0,w,h};
    SDL_RenderDrawRect(render,&re);
    return 0;
}

#define GRID 88
int paint_bg(void* pixels,int pitch,int height,void *param){
    int width=pitch/4;
    Uint32 *p=(Uint32*)pixels;
#ifdef RGBA8888
    const Uint32 bg     = RGBA8888(0x12,0x34,0x56,0x0);
    const Uint32 line   = RGBA8888(0x80,0x80,0x80,0x80);
    const Uint32 border = RGBA8888(0x80,0xff,0x80,0xff);
#else
    SDL_PixelFormat *format= SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    const Uint32 bg     = SDL_MapRGBA(format,0x12,0x34,0x56,0);
    const Uint32 line   = SDL_MapRGBA(format,0x80,0x80,0x80,0x80);
    const Uint32 border = SDL_MapRGBA(format,0x80,0xff,0x80,0xff);
    SDL_FreeFormat(format);
#endif
    int bottom=(height-1)*width;
    for(int ix=0;ix<width;ix++){
        *(p+ix)=border;
        *(p + bottom+ix)=border;
    }
    for(int iy=1;iy<height-1;iy++){
        *(p + iy * width) = border;
        *(p + iy * width+width-1) = border;
        for(int ix=1;ix<width-1;ix++){
            if(((ix % GRID)==0) || ((iy %GRID)==0)){
                *(p + iy * width + ix)=line;
                continue;
            }
            *(p + iy * width + ix)=bg;
        }
    }
    
    return 0;
}
void show_width(SDL_Texture *tex)
{
    int w,h;
    SDL_QueryTexture(tex,NULL,NULL,&w,&h);
    SDL_Log("font texture: %d,%d",w,h);
}

int load_font(CE_APP *app,RESOURCES *res)
{
    SDL_Color c={0x34,0xa7,0x38};
    const char *name=fontname[0];

    TTF_Font *font=TTF_OpenFont(name,30);
    if(font==NULL){
        SDL_LogError(0,"open font %s error %s",name,TTF_GetError());
        return -1;
    }
    if(ce_texture_font(app,&res->tfont, font,c)){
        TTF_CloseFont(font);
        return -1;
    }
    TTF_CloseFont(font);
    return 0;
}
int load_resources(CE_APP *app,RESOURCES *res)
{
    // SDL_Texture *ptt=ce_texture_create_target(app,paint_background,app->dm.w,app->dm.h,NULL);
    // res->background=ptt;
    // if(ptt==NULL){
    //     return -1;
    // }
    int w,h;
    SDL_GetWindowSize(app->win,&w,&h);
    SDL_Texture *ptt=SDL_CreateTexture(app->render,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,w,h);
    if(ptt==NULL){
        return -1;
    }
    if(ce_texture_paint_pixels(ptt,paint_bg,NULL)){
        SDL_DestroyTexture(ptt);
        return -1;
    }
    res->background=ptt;
    return load_font(app,res);
}
void free_res(RESOURCES *res)
{
    SDL_DestroyTexture(res->background);
    SDL_DestroyTexture(res->tfont.font);
}
