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
const char *menustr="开始游戏按键控制难度玩腻了";
const int rng[]={            4,      8,  10,   13};
#define CHAR_COUNT 13
int load_menu(CE_APP *app,RESOURCES *res)
{
    SDL_Color c={0xf3,0xb8,0xf0};
    TTF_Font *font=TTF_OpenFont(fontname[10],56);
    if(font==NULL){
        SDL_LogError(0,"open font %s error %s",fontname[10],TTF_GetError());
        return -1;
    }
    int win_w = app->win_w;
    
    SDL_Texture *tx=ce_texture_create_from_text(app,font,menustr,c);
    if(tx==NULL){
        return -1;
    }
    int w,h;
    SDL_QueryTexture(tx,NULL,NULL,&w,&h);
    w=w/CHAR_COUNT;
    int st=0;
    int y=150;
    for(int ix=0;ix<ITEM_COUNT;ix++){
        res->range[ix].x=st*w;
        res->range[ix].y=0;
        res->range[ix].w=(rng[ix]-st) *w;
        res->range[ix].h=h;
        st=rng[ix];
        SDL_Rect *src=&res->range[ix];
        int x=(win_w-src->w)/2;
        res->range[ix+ITEM_COUNT].x=x;
        res->range[ix+ITEM_COUNT].y=y;
        res->range[ix+ITEM_COUNT].w=src->w;
        res->range[ix+ITEM_COUNT].h=src->h;
        y += 55 + src->h;
    }
    res->menuSel=0;
    res->oldMenuSel=0;
    
    // for(int ix=0;ix<5;ix++){
    //     SDL_Log("range:{%d,%d,%d,%d}",res->range[ix].x,res->range[ix].y,res->range[ix].w,res->range[ix].h);
    // }
    res->menuitem=tx;
    TTF_CloseFont(font);
    return 0;
}
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
    const Uint32 bg     = RGBA8888(0x4b,0x7d,0x91,0xff);
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
    if(load_menu(app,res)){
        return -1;
    }
    return load_font(app,res);
}
void free_res(RESOURCES *res)
{
    SDL_DestroyTexture(res->background);
    SDL_DestroyTexture(res->tfont.font);
}
