#include "appstate.h"


int updateBackground(struct APPSTATE *aps)
{
    Uint8 num=0;
    while(aps->rs->runing){
        SDL_Delay(100);
        num++;
        //SDL_LockMutex(aps->mutex);
        aps->gs->bgcolor=0xff+(num << 8) + ((0xff - num) << 16) + ((num + 0x80)<<24);
        //SDL_UnlockMutex(aps->mutex);
    }
    return 0;
}

int app_run(struct APPSTATE *aps,Uint32 tick)
{
    return 0;
}