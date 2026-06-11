#include <stddef.h>
#include <stdlib.h>
#include "app_mode.h"
#include "font_func.h"
extern const char *const monofont[];
extern const char *const fontname[];
extern const char *const cjkfont[];
const char *strTwenty = "①②③④⑤⑥⑦⑧⑨⑩⑪⑫⑬⑭⑮⑯⑰⑱⑲⑳";
const char *strIcons = "✅❌";
struct SAMPLE_DATA {
    SDL_Texture *textureTwenty;
    SDL_Texture *textureIcons;
};
extern void app_err_push(const char *file, int line, const char *fmt, ...);

static void* sample_mode_init(APP_MODE *mode)
{
    struct SAMPLE_DATA *data = malloc(sizeof(*data));
    if (!data) {
        app_err_push(__FILE__, __LINE__, "Failed to allocate sample data");
        return NULL;
    }
    SDL_Color white = {255, 255, 255, 255};
    data->textureTwenty = create_texture_by_string(mode->renderer, cjkfont[0], 30, strTwenty, white);

    if (!data->textureTwenty) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureTwenty");
        free(data);
        return NULL;
    }
    data->textureIcons = create_texture_by_string(mode->renderer, fontname[0], 24, strIcons, white);
    if (!data->textureIcons) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureIcons");
        SDL_DestroyTexture(data->textureTwenty);
        free(data);
        return NULL;
    }
    return data;
}
int shutdown(void);
static int sample_mode_event(APP_MODE *mode, SDL_Event *event, void *data)
{
    (void)mode;
    (void)data;
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
        shutdown();
    }
    return 0;
}

static int sample_mode_render(APP_MODE *mode, void *data)
{

    struct SAMPLE_DATA *sample_data = data;

    SDL_Rect dst = {0, 0, 0, 0};
    if (SDL_QueryTexture(sample_data->textureTwenty, NULL, NULL, &dst.w, &dst.h) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_QueryTexture Error: %s", SDL_GetError());
        return -1;
    }

    int window_w = 0;
    int window_h = 0;
    SDL_GetWindowSize(mode->window, &window_w, &window_h);
    dst.x = (window_w - dst.w) / 2;
    dst.y = 0;

    if (SDL_RenderCopy(mode->renderer, sample_data->textureTwenty, NULL, &dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }

    if (!sample_data->textureIcons) {
        return 0;
    }

    SDL_Rect icon_dst = {0, 0, 0, 0};
    if (SDL_QueryTexture(sample_data->textureIcons, NULL, NULL, &icon_dst.w, &icon_dst.h) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_QueryTexture Error: %s", SDL_GetError());
        return -1;
    }

    icon_dst.x = (window_w - icon_dst.w) / 2;
    icon_dst.y = dst.y + dst.h;
    if (SDL_RenderCopy(mode->renderer, sample_data->textureIcons, NULL, &icon_dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }

    return 0;
}

static int sample_mode_destroy(APP_MODE *mode, void *data)
{
    (void)mode;
    if (data) {
        free(data);
    }
    return 0;
}

const AbstractModeVTable sample_mode_vtable = {
    .init = sample_mode_init,
    .event = sample_mode_event,
    .render = sample_mode_render,
    .destroy = sample_mode_destroy,
};
