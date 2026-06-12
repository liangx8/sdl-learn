#include <stddef.h>
#include <stdlib.h>
#include "app_mode.h"
#include "texture_func.h"
#include "sample_class.h"

extern const char *const monofont[];
extern const char *const fontname[];
extern const char *const cjkfont[];
const char *strTwenty = "①②③④⑤⑥⑦⑧⑨⑩⑪⑫⑬⑭⑮⑯⑰⑱⑲⑳";
const char *strIcons = "✅❌";
struct SAMPLE_DATA {
    SDL_Texture *textureTwenty;
    SDL_Texture *textureIcons;
    SDL_Texture *textureBackground;
    NUMBER_TEMPLATE *numberTemplate;
    CLASS_DATA classData;
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
    data->textureIcons = create_texture_by_string(mode->renderer, fontname[0], 52, strIcons, white);
    if (!data->textureIcons) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureIcons");
        SDL_DestroyTexture(data->textureTwenty);
        free(data);
        return NULL;
    }
    struct GRID_PARAM grid_param = { .bgcolor = { 230,190, 148, 255 }, .gridsize = 32 };
    data->textureBackground = create_texture_paint_pixels(mode->renderer, 800, 600, grid_texture_callback, &grid_param);
    if (!data->textureBackground) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureBackground");
        SDL_DestroyTexture(data->textureIcons);
        SDL_DestroyTexture(data->textureTwenty);
        free(data);
        return NULL;
    }
    data->numberTemplate = number_template(mode->renderer, monofont[0], &white, 30);
    if (!data->numberTemplate) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create numberTemplate");
        SDL_DestroyTexture(data->textureBackground);
        SDL_DestroyTexture(data->textureIcons);
        SDL_DestroyTexture(data->textureTwenty);
        free(data);
        return NULL;
    }
    populate_random_class1(&data->classData);
    return data;
}
int shutdown(void);
static int sample_mode_event(APP_MODE *mode, SDL_Event *event, void *data)
{
    (void)mode;
    struct SAMPLE_DATA *sample_data = data;

    if (event->type == SDL_KEYUP) {
        switch (event->key.keysym.sym) {
        case SDLK_ESCAPE:
            shutdown();
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            populate_random_class1(&sample_data->classData);
            break;
        default:
            break;
        }
    }
    return 0;
}
int render_sample_class(APP_MODE *mode, const CLASS_DATA *class_data, int x, int y,struct SAMPLE_DATA *sample_data)
{
    if (!mode || !class_data || !sample_data) {
        app_err_push(__FILE__, __LINE__, "render_sample_class: invalid arguments");
        return -1;
    }
    int result = render_number(mode->renderer, sample_data->numberTemplate, class_data->operand[0], x, y);
    if (result != 0) {
        return -1;
    }
    result = render_number(mode->renderer, sample_data->numberTemplate, class_data->operand[1], x + sample_data->numberTemplate->digit_width * 2, y);
    if (result != 0) {
        return -1;
    }
    return 0;
}
static int sample_mode_render(APP_MODE *mode, void *data)
{

    struct SAMPLE_DATA *sample_data = data;

    int window_w = 0;
    int window_h = 0;
    SDL_GetWindowSize(mode->window, &window_w, &window_h);

    SDL_Rect bg_dst = {0, 0, window_w, window_h};
    if (SDL_RenderCopy(mode->renderer, sample_data->textureBackground, NULL, &bg_dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }

    SDL_Rect dst = {0, 0, 0, 0};
    if (SDL_QueryTexture(sample_data->textureTwenty, NULL, NULL, &dst.w, &dst.h) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_QueryTexture Error: %s", SDL_GetError());
        return -1;
    }

    dst.x = (window_w - dst.w) / 2;
    dst.y = 0;

    if (SDL_RenderCopy(mode->renderer, sample_data->textureTwenty, NULL, &dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
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

    /* compute position for rendering numbers below the icons */
    int num_height = sample_data->numberTemplate->digit_height;
    int num_x = (window_w - sample_data->numberTemplate->digit_width * 2) / 2;
    int num_y = icon_dst.y + icon_dst.h;

    if (render_sample_class(mode, &sample_data->classData, num_x, num_y + num_height + 10, sample_data) != 0) {
        app_err_push(__FILE__, __LINE__, "render_sample_class Error");
        return -1;
    }
    return 0;
}

static int sample_mode_destroy(APP_MODE *mode, void *data)
{
    (void)mode;
    if (data) {
        struct SAMPLE_DATA *sample_data = data;
        if (sample_data->textureBackground) {
            SDL_DestroyTexture(sample_data->textureBackground);
        }
        if (sample_data->textureIcons) {
            SDL_DestroyTexture(sample_data->textureIcons);
        }
        if (sample_data->textureTwenty) {
            SDL_DestroyTexture(sample_data->textureTwenty);
        }
        if (sample_data->numberTemplate) {
            destroy_number_template(sample_data->numberTemplate);
        }
        free(sample_data);
    }
    return 0;
}

const AbstractModeVTable sample_mode_vtable = {
    .init = sample_mode_init,
    .event = sample_mode_event,
    .render = sample_mode_render,
    .destroy = sample_mode_destroy,
};
