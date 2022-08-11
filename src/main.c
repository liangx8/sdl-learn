/*
https://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world
https://github.com/Twinklebear/TwinklebearDev-Lessons
*/

#include <SDL.h>
#include <stdio.h>
#include "abs_struct.h"

int main(int argc, char **argv)
{
    struct SDL_APP *app=sdl_create();
    if(app){
        sdl_run(app);
        sdl_free(app);
    }
    /*
    SDL_Surface *surf;
    surf = SDL_CreateRGBSurface(0, 96, 96, 32, 0, 0, 0, 0);
    SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 0, 0));
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    if (tex == NULL)
    {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        printf("SDL_CreateTextureFromSurface error:%s", SDL_GetError());
        return 1;
    }
    for (int i = 0; i < 3; ++i)
    {
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        SDL_Delay(1000);
        printf("second %d\n", i);
    }
    SDL_DestroyTexture(tex); 
    */
    return 0;
}
