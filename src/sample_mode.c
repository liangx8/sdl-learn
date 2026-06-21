#include <stddef.h>
#include <stdlib.h>
#include "app_mode.h"
#include "sdl_app.h"
#include "texture_func.h"
#include "sample_class.h"
const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color COLOR_GRAY = {128, 128, 128, 255};
extern const char *const monofont[];
extern const char *const fontname[];
extern const char *const cjkfont[];
const char *strTwenty = "①②③④⑤⑥⑦⑧⑨⑩⑪⑫⑬⑭⑮⑯⑰⑱⑲⑳";
const char *strCursor = "⬆";
const char *strIcons = "✅❌";
extern void app_err_push(const char *file, int line, const char *fmt, ...);
static struct SAMPLE_DATA sample_data;
int render_sample_class(SdlApp *mode, const CLASS_DATA *class_data, int x, int y);

struct SAMPLE_DATA {
    SDL_Texture *textureTwenty;
    SDL_Texture *textureIcons;
    SDL_Texture *textureBackground;
    SDL_Texture *textureAnswerBackground;
    SDL_Texture *textureCursor;
    int widthCursor;
    int heightCursor;
    int widthTwenty;
    int heightTwenty;
    int widthIcons;
    int heightIcons;
    SDL_Rect rectDstAnswerBackground;
    NUMBER_TEMPLATE *numberTemplate;
    CLASS_DATA classData;
    int (*event)(SdlApp *,SDL_Event *);
    int (*render)(SdlApp *);
};
int number_append(int src,int digit)
{
    if(src<0){
        return digit;
    }
    return src*10+digit;
}
static int _sample_answer_render(SdlApp *app)
{
    if (!app || !app->renderer) {
        app_err_push(__FILE__, __LINE__, "_sample_answer_render: invalid arguments");
        return -1;
    }

    if (!sample_data.textureAnswerBackground || !sample_data.textureIcons) {
        app_err_push(__FILE__, __LINE__, "_sample_answer_render: incomplete sample data");
        return -1;
    }

    if (SDL_RenderCopy(app->renderer, sample_data.textureAnswerBackground, NULL, &sample_data.rectDstAnswerBackground) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }

    SDL_Rect icon_dst = {
        .x = sample_data.rectDstAnswerBackground.x + sample_data.rectDstAnswerBackground.w - sample_data.widthIcons - 20,
        .y = sample_data.rectDstAnswerBackground.y + (sample_data.rectDstAnswerBackground.h - sample_data.heightIcons) / 2,
        .w = sample_data.widthIcons/2,
        .h = sample_data.heightIcons
    };
    SDL_Rect icon_src = {sample_data.widthIcons/2,0,sample_data.widthIcons/2,sample_data.heightIcons};
    SDL_Log("%d + %d = %d | %d",sample_data.classData.operand[0],sample_data.classData.operand[1],sample_data.classData.answer,sample_data.widthIcons);
    if(sample_data.classData.answer==sample_data.classData.operand[0]+sample_data.classData.operand[1]){
        icon_src.x=0;
    }
    if(sample_data.classData.answer>=0){
        if (render_number(app->renderer, sample_data.numberTemplate, sample_data.classData.answer, sample_data.rectDstAnswerBackground.x + 20, sample_data.rectDstAnswerBackground.y + 20) != 0) {
            app_err_push(__FILE__, __LINE__, "render_number Error");
            return -1;
        }
    }

    if (SDL_RenderCopy(app->renderer, sample_data.textureIcons, &icon_src, &icon_dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }

    return 0;
}
static int _sample_answer_event(SdlApp *app,SDL_Event *event);
int shutdown(void);
static int _sample_event(SdlApp *app,SDL_Event *event)
{
    (void)app;
    if (event->type == SDL_KEYUP) {
        switch (event->key.keysym.sym) {
        case SDLK_ESCAPE:
            shutdown();
            break;
        case SDLK_BACKSPACE:
            sample_data.classData.answer =-1;
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if(sample_data.classData.answer>=0){
                sample_data.event=_sample_answer_event;
                sample_data.render=_sample_answer_render;
            }
            break;
        case SDLK_0: case SDLK_1: case SDLK_2: case SDLK_3: case SDLK_4:
        case SDLK_5: case SDLK_6: case SDLK_7: case SDLK_8: case SDLK_9:
            sample_data.classData.answer = number_append(sample_data.classData.answer, event->key.keysym.sym - SDLK_0);
            break;
        case SDLK_KP_0: case SDLK_KP_1: case SDLK_KP_2: case SDLK_KP_3: case SDLK_KP_4:
        case SDLK_KP_5: case SDLK_KP_6: case SDLK_KP_7: case SDLK_KP_8: case SDLK_KP_9:
            sample_data.classData.answer = number_append(sample_data.classData.answer, event->key.keysym.sym - SDLK_KP_0);
            break;
        default:
            break;
        }
    }
    return 0;
}
static int _sample_render(SdlApp *app)
{
    if (!app || !app->window || !app->renderer ) {
        app_err_push(__FILE__, __LINE__, "sample_mode_render: invalid arguments");
        return -1;
    }

    if (!sample_data.textureBackground || !sample_data.textureTwenty || !sample_data.textureIcons || !sample_data.numberTemplate) {
        app_err_push(__FILE__, __LINE__, "sample_mode_render: incomplete sample data");
        return -1;
    }

    int window_w = app->width;
    int window_h = app->height;
    //SDL_GetWindowSize(app->window, &window_w, &window_h);
    //SDL_Log("actual size(%d,%d)",window_w,window_h);
    SDL_Rect bg_dst = {0, 0, window_w, window_h};
    if (SDL_RenderCopy(app->renderer, sample_data.textureBackground, NULL, &bg_dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }

    SDL_Rect dst = { .x = (window_w - sample_data.widthTwenty) / 2, .y = 0, .w = sample_data.widthTwenty, .h = sample_data.heightTwenty };
    if (SDL_RenderCopy(app->renderer, sample_data.textureTwenty, NULL, &dst) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }


    /* compute position for rendering numbers below the icons */
    int num_height = sample_data.numberTemplate->digit_height;
    int num_x = 40;
    int num_y = sample_data.heightTwenty +20;

    if (render_sample_class(app, &sample_data.classData, num_x, num_y + num_height + 10) != 0) {
        app_err_push(__FILE__, __LINE__, "render_sample_class Error");
        return -1;
    }
    if (SDL_RenderCopy(app->renderer, sample_data.textureAnswerBackground, NULL, &sample_data.rectDstAnswerBackground) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_RenderCopy Error: %s", SDL_GetError());
        return -1;
    }
    if(sample_data.classData.answer>=0){
        if (render_number(app->renderer, sample_data.numberTemplate, sample_data.classData.answer, sample_data.rectDstAnswerBackground.x + 20, sample_data.rectDstAnswerBackground.y + 20) != 0) {
            app_err_push(__FILE__, __LINE__, "render_number Error");
            return -1;
        }
    }

    return 0;
}
static int _sample_answer_event(SdlApp *app,SDL_Event *event)
{
    (void)app;
    if(event->type==SDL_KEYUP){
        switch (event->key.keysym.sym)
        {
        case SDLK_ESCAPE:
            shutdown();
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            sample_data.event=_sample_event;
            sample_data.render=_sample_render;
            populate_random_class1(&sample_data.classData);
            break;
        default:
            break;
        }
    }
    return 0;
}
int sample_mode_init(SdlApp *mode)
{
    sample_data.event=_sample_event;
    sample_data.render=_sample_render;
    sample_data.textureTwenty = create_texture_by_string(mode->renderer, cjkfont[0], 30, strTwenty, COLOR_WHITE);

    if (!sample_data.textureTwenty) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureTwenty");
        return -1;
    }
    if(SDL_QueryTexture(sample_data.textureTwenty, NULL, NULL, &sample_data.widthTwenty, &sample_data.heightTwenty)) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to query textureTwenty dimensions");
        SDL_DestroyTexture(sample_data.textureTwenty);
        return -1;
    }

    sample_data.textureIcons = create_texture_by_string(mode->renderer, fontname[0], 80, strIcons, COLOR_WHITE);
    if (!sample_data.textureIcons) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureIcons");
        SDL_DestroyTexture(sample_data.textureTwenty);
        return -1;
    }
    if(SDL_QueryTexture(sample_data.textureIcons, NULL, NULL, &sample_data.widthIcons, &sample_data.heightIcons)) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to query textureIcons dimensions");
        SDL_DestroyTexture(sample_data.textureIcons);
        SDL_DestroyTexture(sample_data.textureTwenty);
        return -1;
    }
    struct GRID_PARAM grid_param = { .bgcolor = { 230,190, 148, 255 }, .gridsize = 32 };
    sample_data.textureBackground = create_texture_paint_pixels(mode->renderer, 800, 600, grid_texture_callback, &grid_param);
    if (!sample_data.textureBackground) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureBackground");
        SDL_DestroyTexture(sample_data.textureIcons);
        SDL_DestroyTexture(sample_data.textureTwenty);
        return -1;
    }
    sample_data.numberTemplate = number_template(mode->renderer, monofont[0], &COLOR_WHITE, 30);
    if (!sample_data.numberTemplate) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create numberTemplate");
        SDL_DestroyTexture(sample_data.textureBackground);
        SDL_DestroyTexture(sample_data.textureIcons);
        SDL_DestroyTexture(sample_data.textureTwenty);
        return -1;
    }
    sample_data.rectDstAnswerBackground = (SDL_Rect){.x = 20, .y = 400, .w = 760, .h = 150};
    SDL_Color answer_bgcolor = COLOR_GRAY;
    sample_data.textureAnswerBackground = create_texture_paint_pixels(mode->renderer, sample_data.rectDstAnswerBackground.w, sample_data.rectDstAnswerBackground.h, rect_texture_callback, &answer_bgcolor);
    if (!sample_data.textureAnswerBackground) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureAnswerBackground");
        destroy_number_template(sample_data.numberTemplate);
        SDL_DestroyTexture(sample_data.textureBackground);
        SDL_DestroyTexture(sample_data.textureIcons);
        SDL_DestroyTexture(sample_data.textureTwenty);
        return -1;
    }
    sample_data.textureCursor=create_texture_by_string(mode->renderer,cjkfont[0],30,strCursor,COLOR_GRAY);
    if (!sample_data.textureTwenty) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to create textureTwenty");
        destroy_number_template(sample_data.numberTemplate);
        SDL_DestroyTexture(sample_data.textureBackground);
        SDL_DestroyTexture(sample_data.textureIcons);
        SDL_DestroyTexture(sample_data.textureTwenty);
        SDL_DestroyTexture(sample_data.textureAnswerBackground);
        return -1;
    }
    if(SDL_QueryTexture(sample_data.textureCursor, NULL, NULL, &sample_data.widthCursor, &sample_data.heightCursor)) {
        app_err_push(__FILE__, __LINE__, "sample_mode_init: failed to query textureCursor dimensions");
        SDL_DestroyTexture(sample_data.textureBackground);
        SDL_DestroyTexture(sample_data.textureIcons);
        SDL_DestroyTexture(sample_data.textureTwenty);
        SDL_DestroyTexture(sample_data.textureCursor);
        return -1;
    }

    populate_random_class1(&sample_data.classData);
    return 0;
}

static int sample_mode_event(SdlApp *mode, SDL_Event *event)
{
    return sample_data.event(mode,event);
}
static int sample_mode_pause(SdlApp *mode)
{
    (void)mode;
    return 0;
}
static int sample_mode_resume(SdlApp *mode)
{    (void)mode;
    return 0;
}
int render_sample_class(SdlApp *mode, const CLASS_DATA *class_data, int x, int y)
{
    if (!mode || !class_data) {
        app_err_push(__FILE__, __LINE__, "render_sample_class: invalid arguments");
        return -1;
    }
    int result = render_number(mode->renderer, sample_data.numberTemplate, class_data->operand[0], x, y);
    if (result != 0) {
        return -1;
    }
    result = render_symbol(mode->renderer, sample_data.numberTemplate, '=', x + sample_data.numberTemplate->digit_width * 2, y);
    if (result != 0) {
        return -1;
    }
    result = render_number(mode->renderer, sample_data.numberTemplate, class_data->operand[1], x + sample_data.numberTemplate->digit_width * 4, y);
    if (result != 0) {
        return -1;
    }
    result = render_symbol(mode->renderer, sample_data.numberTemplate, '+', x + sample_data.numberTemplate->digit_width * 6, y);
    if (result != 0) {
        return -1;
    }
    return 0;
}

static int sample_mode_render(SdlApp *mode)
{
    return sample_data.render(mode);
}

static int sample_mode_destroy(SdlApp *mode)
{
    (void)mode;
    if (sample_data.textureBackground) {
        SDL_DestroyTexture(sample_data.textureBackground);
    }
    if (sample_data.textureIcons) {
        SDL_DestroyTexture(sample_data.textureIcons);
    }
    if (sample_data.textureTwenty) {
        SDL_DestroyTexture(sample_data.textureTwenty);
    }
    if (sample_data.numberTemplate) {
        destroy_number_template(sample_data.numberTemplate);
    }
    return 0;
}

const AbstractActionVTable sample_mode_vtable = {
    .init = sample_mode_init,
    .event = sample_mode_event,
    .render = sample_mode_render,
    .destroy = sample_mode_destroy,
    .pause = sample_mode_pause,
    .resume = sample_mode_resume
};
