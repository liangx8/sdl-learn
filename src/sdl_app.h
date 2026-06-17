#ifndef SDL_APP_H
#define SDL_APP_H

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SdlApp {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width;
    int height;
    int running;
} SdlApp;

SdlApp* sdl_app_create(int (*sdl_init)(),const char *title, int width, int height);
void sdl_app_destroy(SdlApp* app);
void sdl_app_run(SdlApp* app);
void sdl_app_cleanup(SdlApp* app);

#ifdef __cplusplus
}
#endif

#endif // SDL_APP_H
