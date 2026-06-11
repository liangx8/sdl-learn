#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "font_func.h"
#include "app_err.h"
#include "app_mode.h"

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const char *WELCOME_TEXT = "SDL2 例程";

int shutdown(void)
{
    SDL_Event event;
    event.type = SDL_QUIT;
    if (SDL_PushEvent(&event) != 1) {
        app_err_push(__FILE__, __LINE__, "SDL_PushEvent Error: %s", SDL_GetError());
        return 1;
    }
    return 0;
}

extern const AbstractModeVTable sample_mode_vtable;
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        app_err_push(__FILE__, __LINE__, "TTF_Init Error: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        WELCOME_TEXT,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        app_err_push(__FILE__, __LINE__, "SDL_CreateWindow Error: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        app_err_push(__FILE__, __LINE__, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    APP_MODE mode = {
        .vtable = &sample_mode_vtable,
        .window = window,
        .renderer = renderer
    };
    void *mode_data = sample_mode_vtable.init(&mode);
    while (1) {
        SDL_Event event;
        if (SDL_WaitEvent(&event)==0) {
            app_err_push(__FILE__, __LINE__, "SDL_WaitEvent Error: %s", SDL_GetError());
            break;
        }
        if (event.type == SDL_QUIT) {
            break;
        }
        if (mode.vtable->event(&mode, &event, mode_data) != 0) {
            app_err_push(__FILE__, __LINE__, "Mode event handler error");
            break;
        }
        SDL_RenderClear(renderer);
        if (mode.vtable->render(&mode, mode_data) != 0) {
            app_err_push(__FILE__, __LINE__, "Mode render handler error");
            break;
        }
        SDL_RenderPresent(renderer);
    }
    mode.vtable->destroy(&mode, mode_data);
    err_stack_print();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}