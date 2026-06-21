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
    const APP_LAYOUT *layout,
    const char *title, int width, int height)
{
    if(layout==NULL||layout->sdl_init==NULL||layout->sdl_quit==NULL){
        return NULL;
    }
    struct _sdlapp_internal *app_intr = (struct _sdlapp_internal *)malloc(sizeof(struct _sdlapp_internal));
    if (!app_intr) {
        return NULL;
    }
    SdlApp *app=(SdlApp*)app_intr;
    app->window = NULL;
    app->renderer = NULL;
    app->layout=layout;
    app_intr->mode_stack=list_create(20);
    if(layout->sdl_init()){
        list_destroy(app_intr->mode_stack);
        free(app);
        return NULL;
    }
    if(SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &app->window, &app->renderer) != 0) {
        layout->sdl_quit();
        list_destroy(app_intr->mode_stack);
        free(app);
        return NULL;
    }
    if(layout->app_init && (layout->app_init(app)!=0)){
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        list_destroy(app_intr->mode_stack);
        free(app);
        layout->sdl_quit();
        return NULL;
    }
    app->width = width;
    app->height = height;
    SDL_Log("expect size (%d,%d)",width,height);
    SDL_SetWindowTitle(app->window, title ? title : "SDL App");
    return app;
}


void sdl_app_destroy(SdlApp *app)
{
    if (!app) {
        return;
    }
    struct _sdlapp_internal *app_intr=(struct _sdlapp_internal *)app;
    if(app->layout->app_destroy){
        app->layout->app_destroy(app);
    }
    if (app->renderer) {
        SDL_DestroyRenderer(app->renderer);
    }

    if (app->window) {
        SDL_DestroyWindow(app->window);
    }
    app->layout->sdl_quit();
    list_destroy(app_intr->mode_stack);
    free(app);
}

int shutdown(void){
    SDL_Event event;
    event.type = SDL_QUIT;
    if (SDL_PushEvent(&event) != 1) {
        app_err_push(__FILE__, __LINE__, "SDL_PushEvent Error: %s", SDL_GetError());
        return 1;
    }
    return 0;
}
int sdl_app_run(SdlApp *app,const AbstractActionVTable *startup)
{
    struct _sdlapp_internal *app_intr=(struct _sdlapp_internal *)app;
    list_push(app_intr->mode_stack,startup);
    SDL_Event event;
    int retval=0;
    while (1) {
        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
        } else {
            app_err_push(__FILE__, __LINE__, "SDL_WaitEvent Error: %s", SDL_GetError());
            retval=-1;
            goto err_exit;
        }
        startup->event(app,&event);
        startup->render(app);
        SDL_RenderPresent(app->renderer);
    }
err_exit:
    while(1){
        const AbstractActionVTable *st=list_pop(app_intr->mode_stack);
        if(st==NULL){
            break;
        }
        st->destroy(app);
    }
    return retval;
}
