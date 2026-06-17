#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "sdl_app.h"

SdlApp *sdl_app_create(int (*sdl_init)(),const char *title, int width, int height)
{
    SdlApp *app = (SdlApp *)malloc(sizeof(SdlApp));
    if (!app) {
        return NULL;
    }

    app->window = NULL;
    app->renderer = NULL;
    app->running = false;

    if(sdl_init && sdl_init() != 0) {
        free(app);
        return NULL;
    }
    if(SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_SHOWN, &app->window, &app->renderer) != 0) {
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

    if (app->renderer) {
        SDL_DestroyRenderer(app->renderer);
    }

    if (app->window) {
        SDL_DestroyWindow(app->window);
    }

    SDL_Quit();
    free(app);
}

void sdl_app_poll_events(SdlApp *app)
{
    if (!app) {
        return;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            app->running = false;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            app->running = false;
        }
    }
}

void sdl_app_run(SdlApp *app)
{
    if (!app || !app->renderer) {
        return;
    }

    while (app->running) {
        sdl_app_poll_events(app);

        SDL_SetRenderDrawColor(app->renderer, 16, 24, 48, 255);
        SDL_RenderClear(app->renderer);

        SDL_SetRenderDrawColor(app->renderer, 200, 200, 220, 255);
        SDL_Rect rect = {app->width / 4, app->height / 4, app->width / 2, app->height / 2};
        SDL_RenderFillRect(app->renderer, &rect);

        SDL_RenderPresent(app->renderer);
        SDL_Delay(16);
    }
}
