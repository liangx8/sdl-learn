#ifndef SDL_APP_H
#define SDL_APP_H

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SdlApp {
    SDL_Window* window;
    SDL_Renderer* renderer;
    void *app_data;
    void (*before_destroy)(void *app_data);
    int width;
    int height;
    int running;// @deprecated
} SdlApp;

SdlApp* sdl_app_create(
    int (*sdl_init)(void **app_data),
    void (*before_destroy)(void *app_data),
    const char *title, 
    int width, 
    int height);
void sdl_app_destroy(SdlApp* app);
int sdl_app_run(SdlApp* app);

#ifdef __cplusplus
}
#endif

#endif // SDL_APP_H
