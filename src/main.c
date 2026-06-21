#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "app_err.h"
#include "sdl_app.h"
#include "app_mode.h"

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const char *WELCOME_TEXT = "SDL2 例程🌍";




int main_sdl_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        app_err_push(__FILE__, __LINE__, "SDL_Init Error: %s", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        app_err_push(__FILE__, __LINE__, "TTF_Init Error: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    return 0;
}
void main_sdl_quit(void)
{
    TTF_Quit();
    SDL_Quit();
}
const APP_LAYOUT layout={
    .app_destroy=NULL,
    .app_init=NULL,
    .sdl_init=main_sdl_init,
    .sdl_quit=main_sdl_quit
};
extern const AbstractActionVTable sample_mode_vtable;
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    SdlApp *app=sdl_app_create(&layout,WELCOME_TEXT,WINDOW_WIDTH,WINDOW_HEIGHT);
    if(app==NULL){
        err_stack_print();
        return -1;
    }
    sample_mode_vtable.init(app);
    sdl_app_run(app,&sample_mode_vtable);
    sdl_app_destroy(app);
    return 0;
}