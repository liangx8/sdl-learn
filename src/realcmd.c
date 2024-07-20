#include <SDL2/SDL_log.h>
int cmd_sample(void *data)
{
    SDL_Log("Ticks %ld",(long)data);
    return 0;
}