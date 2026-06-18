#ifndef SDL_APP_H
#define SDL_APP_H

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _SdlApp SdlApp;
typedef struct _app_layout{
    int (*sdl_init)(void);
    void (*sdl_quit)(void);
    int (*app_init)(SdlApp *);
    void (*app_destroy)(SdlApp *);
} APP_LAYOUT;

typedef struct _stage STAGE;
typedef struct _SdlApp {
    const APP_LAYOUT *layout;
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width;
    int height;
};

SdlApp* sdl_app_create(
    APP_LAYOUT *layout,
    const char *title, 
    int width, 
    int height);
void sdl_app_destroy(SdlApp* app);
int sdl_app_run(SdlApp* app,STAGE *);

#ifdef __cplusplus
}
#endif

#endif // SDL_APP_H
