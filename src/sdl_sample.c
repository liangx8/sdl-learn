#include <SDL.h>
#include <malloc.h>

struct app_data{
    SDL_Window      *win;
    SDL_Renderer    *ren;
};

struct app_data *sdl_create(void)
{
    struct app_data *psd;
    SDL_Window      *win;
    SDL_Renderer    *ren;
    //First we need to start up SDL, and make sure it went ok
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL init error %s\n", SDL_GetError());
        return NULL;
    }
    win=SDL_CreateWindow("SDL教程",100,100,640,480,SDL_WINDOW_SHOWN);
    if(win == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        goto error_exit;
    }
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL)
    {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        goto error_exit;
    }


    psd=malloc(sizeof(struct app_data));
    psd->win=win;
    psd->ren=ren;
    return psd;
error_exit:
    SDL_Quit();
    return NULL;
}
void sdl_cleanup(struct app_data *ptr)
{
    SDL_DestroyRenderer(ptr->ren);
    SDL_DestroyWindow(ptr->win);
    SDL_Quit();
    free(ptr);
}
void sdl_run(struct app_data *pd)
{
    SDL_Surface *surf;
    surf = SDL_CreateRGBSurface(0, 50, 50, 32, 0, 0, 0, 0);
    SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0xea, 0xea, 0xea));
    SDL_Texture *tex = SDL_CreateTextureFromSurface(pd->ren, surf);
    if (tex == NULL)
    {
        sdl_cleanup(pd);
        printf("SDL_CreateRGBSurface error:%s", SDL_GetError());
        return;
    }
    for (int i = 0; i < 10; ++i)
    {
        SDL_RenderClear(pd->ren);
        SDL_RenderCopy(pd->ren, tex, NULL, NULL);
        SDL_RenderPresent(pd->ren);
        SDL_Delay(1000);
        printf("second %d\n", i);
    }
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}