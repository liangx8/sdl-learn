#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "app_mode.h"
#include "sdl_app.h"
#include "app_err.h"
#include "list.h"
struct _sdlapp_internal{
    SdlApp app;
    LIST *mode_stack;
};
SdlApp *sdl_app_create(
    int (*sdl_init)(void **app_data),// 应该调用SDL_Init(),TTF_Init(),PNG_Init()等，包括创建用户数据
    void (*before_destroy)(void *app_data),//调用SDL_Quit(),TTF_Quit(),PNG_Quit()
    const char *title, int width, int height)
{
    struct _sdlapp_internal *app_intr = (struct _sdlapp_internal *)malloc(sizeof(struct _sdlapp_internal));
    if (!app_intr) {
        return NULL;
    }
    SdlApp *app=(SdlApp*)app_intr;

    app->window = NULL;
    app->renderer = NULL;
    app->running = false;
    app->before_destroy = before_destroy;
    app_intr->mode_stack=list_create(20);

    if(sdl_init && sdl_init(&app->app_data) != 0) {
        free(app);
        return NULL;
    }
    if(SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &app->window, &app->renderer) != 0) {
        if(app->before_destroy){
            app->before_destroy(app->app_data);
        }
        free(app);
        return NULL;
    }
    app->width = width;
    app->height = height;
    SDL_SetWindowTitle(app->window, title ? title : "SDL App");
    app->running = true;
    return app;
}


void sdl_app_destroy(SdlApp *app)
{
    if (!app) {
        return;
    }
    struct _sdlapp_internal *app_intr=(struct _sdlapp_internal *)app;
    if (app->renderer) {
        SDL_DestroyRenderer(app->renderer);
    }

    if (app->window) {
        SDL_DestroyWindow(app->window);
    }

    app->before_destroy(app->app_data);
    list_destroy(app_intr->mode_stack);
    free(app);
}


int sdl_app_run(SdlApp *app,STAGE *startup)
{
    if (!app || !app->renderer) {
        return -1;
    }
    struct _sdlapp_internal *app_intr=(struct _sdlapp_internal *)app;
    list_push(app_intr->mode_stack,startup);
    STAGE *cur=startup;
    SDL_Event event;
    while (1) {
        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
        } else {
            app_err_push(__FILE__, __LINE__, "SDL_WaitEvent Error: %s", SDL_GetError());
            return -1;
        }
        cur->action->event(app,&event,cur->stage_data);
        cur->action->render(app,cur->stage_data);

    }
    return 0;
}
