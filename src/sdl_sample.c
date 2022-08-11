#include <SDL.h>
#include <malloc.h>

struct sdl_data{
    SDL_Window      *win;
    SDL_Renderer    *ren;
};

struct sdl_data *sdl_create(void)
{
    struct sdl_data *psd;
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


    psd=malloc(sizeof(struct sdl_data));
    psd->win=win;
    psd->ren=ren;
    return psd;
error_exit:
    SDL_Quit();
    return NULL;
}
void sdl_free(struct sdl_data *ptr)
{
    SDL_DestroyRenderer(ptr->ren);
    SDL_DestroyWindow(ptr->win);
    free(ptr);
}
void sdl_run(struct sdl_data *pd)
{
    SDL_Surface *surf;
    surf = SDL_CreateRGBSurface(0, 96, 96, 32, 0, 0, 0, 0);
    SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 0, 0));
    SDL_Texture *tex = SDL_CreateTextureFromSurface(pd->ren, surf);
    SDL_FreeSurface(surf);
    if (tex == NULL)
    {
        SDL_DestroyRenderer(pd->ren);
        SDL_DestroyWindow(pd->win);
        SDL_Quit();
        printf("SDL_CreateTextureFromSurface error:%s", SDL_GetError());
        return;
    }
    for (int i = 0; i < 3; ++i)
    {
        SDL_RenderClear(pd->ren);
        SDL_RenderCopy(pd->ren, tex, NULL, NULL);
        SDL_RenderPresent(pd->ren);
        SDL_Delay(1000);
        printf("second %d\n", i);
    }
    SDL_DestroyTexture(tex); 

}