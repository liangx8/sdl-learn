#pragma once
#include <SDL3/SDL.h>
typedef int (*TexturePaintPixels)(char *,int,int,void *);
SDL_Texture* cpl_create_texture_paint_pixels(SDL_Renderer *ren,int w,int h,TexturePaintPixels tpp, void *param);
