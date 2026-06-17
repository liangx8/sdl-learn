#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "app_err.h"
#include "sdl_app.h"

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const char *WELCOME_TEXT = "SDL2 例程🌍";




int app_init(void **ref)
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
    *ref=NULL;
    return 0;
}
void before_destroy(void *data)
{
    (void)data;
    TTF_Quit();
    SDL_Quit();
}
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    SdlApp *app=sdl_app_create(app_init,before_destroy,WELCOME_TEXT,WINDOW_WIDTH,WINDOW_HEIGHT);
    if(app==NULL){
        err_stack_print();
        return -1;
    }
    sdl_app_run(app);
    sdl_app_destroy(app);
    return 0;
}