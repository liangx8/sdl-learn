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
        sdl_cleanup(app);
    }

    return 0;
}
