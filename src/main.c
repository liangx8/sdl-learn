/*
https://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world
https://github.com/Twinklebear/TwinklebearDev-Lessons
*/

#ifdef TEST_MAIN
#include <SDL2/SDL.h>
#include <stdio.h>
#include "cengin/ce_app.h"
#define EXECUTE() test()

int run(void *pl)
{
    printf("come with msg %s\n",(const char *)pl);
    return 0;
}
const char *hello="hello";
void test(){
    THREAD_CMD cmds;
    cmds_init(&cmds);
    SDL_Log("no 1 %lx/%lx",(long)run,(long)hello);
    cmds_put(&cmds,run,(void *)hello);
    SDL_Delay(1000);
    SDL_Log("no 2");
    cmds_put(&cmds,run,(void *)"你好");
    SDL_Delay(1000);
    SDL_Log("no 3");
    cmds_put(&cmds,run,(void *)"ありがとうございます");
    cmds_close(&cmds);
}
#else
#define EXECUTE() sdl_sample()
void sdl_sample();
#endif

int main(int argc, char **argv)
{
    EXECUTE();

    return 0;
}
