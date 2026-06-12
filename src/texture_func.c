#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "app_err.h"
#include "texture_func.h"

SDL_Texture* create_texture_paint_pixels(SDL_Renderer *ren, int w, int h,
                                         texture_paint_callback pt, void *param)
{
    // 回调函数中的pitch是y轴的字节数，因为每像素要用rgba来描述，所以要４字节
    SDL_Texture *ttr=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,w,h);
    SDL_Log("创建Texture(width:%d,height:%d)",w,h);
    char *pixels;
    int pitch;
    if(ttr==NULL){
        app_err_push(__FILE__, __LINE__, "SDL_CreateTexture Error: %s", SDL_GetError());
        return NULL;
    }
    if(SDL_LockTexture(ttr,NULL,(void **)&pixels,&pitch)){
        app_err_push(__FILE__, __LINE__, "SDL_LockTexture Error: %s", SDL_GetError());
        SDL_DestroyTexture(ttr);
        return NULL;
    }
    if(pt(pixels,h,pitch,param)){
        app_err_push(__FILE__, __LINE__, "create_texture_paint_pixels callback failed");
        SDL_UnlockTexture(ttr);
        SDL_DestroyTexture(ttr);
        return NULL;
    }
    SDL_UnlockTexture(ttr);
    return ttr;
}

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

// fonts



SDL_Texture *create_texture_by_string(SDL_Renderer *renderer, const char *font_path, int size, const char *text, SDL_Color color)
{
    if (!renderer || !font_path || !text || size <= 0) {
        app_err_push(__FILE__, __LINE__, "create_texture_by_string: invalid arguments");
        return NULL;
    }

    TTF_Font *font = TTF_OpenFont(font_path, size);
    if (!font) {
        app_err_push(__FILE__, __LINE__, "TTF_OpenFont Error: %s", TTF_GetError());
        return NULL;
    }

    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) {
        app_err_push(__FILE__, __LINE__, "TTF_RenderUTF8_Blended Error: %s", TTF_GetError());
        TTF_CloseFont(font);
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        app_err_push(__FILE__, __LINE__, "SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
    }

    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    return texture;
}

NUMBER_TEMPLATE *number_template(SDL_Renderer *renderer,const char *font_path, const SDL_Color *color,int size){
    if (!renderer || !font_path || !color || size <= 0) {
        app_err_push(__FILE__, __LINE__, "number_template: invalid arguments");
        return NULL;
    }

    SDL_Texture *texture = create_texture_by_string(renderer, font_path, size, "0123456789=+-", *color);
    if (!texture) {
        return NULL;
    }

    int tex_w = 0;
    int tex_h = 0;
    if (SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_QueryTexture Error: %s", SDL_GetError());
        SDL_DestroyTexture(texture);
        return NULL;
    }

    if (tex_w <= 0 || tex_h <= 0) {
        app_err_push(__FILE__, __LINE__, "number_template: invalid texture dimensions");
        SDL_DestroyTexture(texture);
        return NULL;
    }

    NUMBER_TEMPLATE *tmpl = (NUMBER_TEMPLATE *)SDL_malloc(sizeof(NUMBER_TEMPLATE));
    if (!tmpl) {
        app_err_push(__FILE__, __LINE__, "number_template: allocation failed");
        SDL_DestroyTexture(texture);
        return NULL;
    }

    tmpl->texture = texture;
    tmpl->digit_width = tex_w / 13; // 10 digits + '+' + '=' + '-'
    tmpl->digit_height = tex_h;
    return tmpl;
}

void destroy_number_template(NUMBER_TEMPLATE *tmpl){
    if (!tmpl) {
        return;
    }
    if (tmpl->texture) {
        SDL_DestroyTexture(tmpl->texture);
    }
    SDL_free(tmpl);
}


int render_number(SDL_Renderer *renderer,const NUMBER_TEMPLATE *tmpl,int number,int x,int y){
    if (!renderer || !tmpl || number < 0) {
        app_err_push(__FILE__, __LINE__, "render_number: invalid arguments");
        return -1;
    }

    if (!tmpl->texture || tmpl->digit_width <= 0 || tmpl->digit_height <= 0) {
        app_err_push(__FILE__, __LINE__, "render_number: invalid template");
        return -1;
    }

    char digits[32];
    int length = SDL_snprintf(digits, sizeof(digits), "%d", number);
    if (length <= 0 || length >= (int)sizeof(digits)) {
        app_err_push(__FILE__, __LINE__, "render_number: invalid number conversion");
        return -1;
    }

    for (int i = 0; i < length; ++i) {
        char ch = digits[i];
        if (ch < '0' || ch > '9') {
            continue;
        }

        int index = ch - '0';
        SDL_Rect src = { index * tmpl->digit_width, 0, tmpl->digit_width, tmpl->digit_height };
        SDL_Rect dst = { x + i * tmpl->digit_width, y, tmpl->digit_width, tmpl->digit_height };

        if (SDL_RenderCopy(renderer, tmpl->texture, &src, &dst) != 0) {
            app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
            return -1;
        }
    }

    return 0;
}
