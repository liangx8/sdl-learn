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
SDL_Texture* cpl_create_texture_paint_pixels(SDL_Renderer*,int,int,typeof(int (*)(char *,int,int,void *)), void *);

int cpl_render_ascii(SDL_Renderer *ren,struct TEXTURE_ASCII *tasc,const char *text,int x,int y);
SDL_Texture *cpl_create_texture_text(SDL_Renderer *ren,const char *fontname,const char *str,SDL_Color color,int size);
int rect_texture_callback(char *,int,int,void *);
struct GRID_PARAM{
    // 底颜色
    SDL_Color bgcolor;
    int gridsize;
};
int grid_texture_callback(char *pixels,int h,int pitch,void *param);