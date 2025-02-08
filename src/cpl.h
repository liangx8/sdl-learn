#pragma once
#include <SDL2/SDL.h>
/**
 * @brief 储存0x21到0x7e到字符
 */
struct TEXTURE_ASCII{
    SDL_Texture *texture;
    int w; // 每个字摸的宽度
    int h; // 高度
};
int cpl_create_texture_ascii(SDL_Renderer *ren,const char *fontname,SDL_Color color,int size,struct TEXTURE_ASCII *tasc);
int cpl_create_texture_ascii_ucs2(SDL_Renderer *ren,const char *fontname,SDL_Color color,int size,struct TEXTURE_ASCII *tasc);
typedef int (*TexturePaintPixels)(char *,int,int,void *);
SDL_Texture* cpl_create_texture_paint_pixels(SDL_Renderer *ren,int w,int h,TexturePaintPixels tpp, void *param);

int cpl_render_ascii(SDL_Renderer *ren,struct TEXTURE_ASCII *tasc,const char *text,int x,int y);