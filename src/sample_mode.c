#include <stddef.h>
#include <stdlib.h>
#include "app_mode.h"
#include "texture_func.h"
#include "sample_class.h"
const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color COLOR_GRAY = {128, 128, 128, 255};
extern const char *const monofont[];
extern const char *const fontname[];
extern const char *const cjkfont[];
const char *strTwenty = "①②③④⑤⑥⑦⑧⑨⑩⑪⑫⑬⑭⑮⑯⑰⑱⑲⑳";
const char *strIcons = "✅❌";
struct SAMPLE_DATA {
    SDL_Texture *textureTwenty;
    SDL_Texture *textureIcons;
    SDL_Texture *textureBackground;
    SDL_Texture *textureAnswerBackground;
    SDL_Rect rectTwenty;
    SDL_Rect rectIcons;
    SDL_Rect rectDstAnswerBackground;
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
    data->textureTwenty = create_texture_by_string(mode->renderer, cjkfont[0], 30, strTwenty, COLOR_WHITE);

    if (!data->textureTwenty) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureTwenty");
        free(data);
        return NULL;
    }
    if(SDL_QueryTexture(data->textureTwenty, NULL, NULL, &data->rectTwenty.w, &data->rectTwenty.h)) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to query textureTwenty dimensions");
        SDL_DestroyTexture(data->textureTwenty);
        free(data);
        return NULL;
    }

    data->textureIcons = create_texture_by_string(mode->renderer, fontname[0], 52, strIcons, COLOR_WHITE);
    if (!data->textureIcons) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureIcons");
        SDL_DestroyTexture(data->textureTwenty);
        free(data);
        return NULL;
    }
    if(SDL_QueryTexture(data->textureIcons, NULL, NULL, &data->rectIcons.w, &data->rectIcons.h)) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to query textureIcons dimensions");
        SDL_DestroyTexture(data->textureIcons);
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
    data->numberTemplate = number_template(mode->renderer, monofont[0], &COLOR_WHITE, 30);
    if (!data->numberTemplate) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create numberTemplate");
        SDL_DestroyTexture(data->textureBackground);
        SDL_DestroyTexture(data->textureIcons);
        SDL_DestroyTexture(data->textureTwenty);
        free(data);
        return NULL;
    }
    data->rectDstAnswerBackground = (SDL_Rect){.x = 20, .y = 400, .w = 760, .h = 150};
    SDL_Color answer_bgcolor = COLOR_GRAY;
    data->textureAnswerBackground = create_texture_paint_pixels(mode->renderer, data->rectDstAnswerBackground.w, data->rectDstAnswerBackground.h, rect_texture_callback, &answer_bgcolor);
    if (!data->textureAnswerBackground) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureAnswerBackground");
        destroy_number_template(data->numberTemplate);
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
        case SDLK_0: case SDLK_1: case SDLK_2: case SDLK_3: case SDLK_4:
        case SDLK_5: case SDLK_6: case SDLK_7: case SDLK_8: case SDLK_9:
            sample_data->classData.answer = event->key.keysym.sym - SDLK_0;
            break;
        case SDLK_KP_0: case SDLK_KP_1: case SDLK_KP_2: case SDLK_KP_3: case SDLK_KP_4:
        case SDLK_KP_5: case SDLK_KP_6: case SDLK_KP_7: case SDLK_KP_8: case SDLK_KP_9:
            sample_data->classData.answer = event->key.keysym.sym - SDLK_KP_0;
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
    result = render_symbol(mode->renderer, sample_data->numberTemplate, '=', x + sample_data->numberTemplate->digit_width * 2, y);
    if (result != 0) {
        return -1;
    }
    result = render_number(mode->renderer, sample_data->numberTemplate, class_data->operand[1], x + sample_data->numberTemplate->digit_width * 4, y);
    if (result != 0) {
        return -1;
    }
    result = render_symbol(mode->renderer, sample_data->numberTemplate, '+', x + sample_data->numberTemplate->digit_width * 6, y);
    if (result != 0) {
        return -1;
    }
    return 0;
}
static int sample_mode_render(APP_MODE *mode, void *data)
{
    if (!mode || !mode->window || !mode->renderer || !data) {
        app_err_push(__FILE__, __LINE__, "sample_mode_render: invalid arguments");
        return -1;
    }

    struct SAMPLE_DATA *sample_data = data;
    if (!sample_data->textureBackground || !sample_data->textureTwenty || !sample_data->textureIcons || !sample_data->numberTemplate) {
        app_err_push(__FILE__, __LINE__, "sample_mode_render: incomplete sample data");
        return -1;
    }

    int window_w = 0;
    int window_h = 0;
    SDL_GetWindowSize(mode->window, &window_w, &window_h);

    SDL_Rect bg_dst = {0, 0, window_w, window_h};
    if (SDL_RenderCopy(mode->renderer, sample_data->textureBackground, NULL, &bg_dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }

    SDL_Rect dst = sample_data->rectTwenty;
    dst.x = (window_w - dst.w) / 2;
    dst.y = 0;
    if (SDL_RenderCopy(mode->renderer, sample_data->textureTwenty, NULL, &dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }

    SDL_Rect icon_dst = sample_data->rectIcons;
    icon_dst.x = (window_w - icon_dst.w) / 2;
    icon_dst.y = dst.y + dst.h;
    if (SDL_RenderCopy(mode->renderer, sample_data->textureIcons, NULL, &icon_dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }

    /* compute position for rendering numbers below the icons */
    int num_height = sample_data->numberTemplate->digit_height;
    int num_x = 40;
    int num_y = icon_dst.y + icon_dst.h;

    if (render_sample_class(mode, &sample_data->classData, num_x, num_y + num_height + 10, sample_data) != 0) {
        app_err_push(__FILE__, __LINE__, "render_sample_class Error");
        return -1;
    }
    if (SDL_RenderCopy(mode->renderer, sample_data->textureAnswerBackground, NULL, &sample_data->rectDstAnswerBackground) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }
    if(sample_data->classData.answer>=0){
        if (render_number(mode->renderer, sample_data->numberTemplate, sample_data->classData.answer, sample_data->rectDstAnswerBackground.x + 20, sample_data->rectDstAnswerBackground.y + 20) != 0) {
            app_err_push(__FILE__, __LINE__, "render_number Error");
            return -1;
        }
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
