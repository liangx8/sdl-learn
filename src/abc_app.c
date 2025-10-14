#include "abc.h"
#include "map.h"
STAGE *curStage;
MAP keymap;

int switch_stage(STAGE *next)
{
    MINUS_ERR(curStage->action->dettech(NULL));
    MINUS_ERR(next->action->attach(keymap));
    curStage=next;
    return 0;

}
const Uint32 FP=1000/60;
#define ERR_EXIT(x) if(x) goto err_exit
/**
 * @brief app_run() must run in thread what initial video
 */
int app_run(RUNSTATE *rs,STAGE *start)
{
    SDL_Event ev;
    curStage=start;
    keymap=map_new(64);
    start->action->attach(keymap);
    //rs->present=1;
    SDL_RenderPresent(rs->ren);
    Uint32 tick=0;
    while(SDL_WaitEvent(&ev)){
        if(ev.type >= SDL_USEREVENT){
            ERR_EXIT(curStage->action->userEvent(&ev));
        } else {

            switch (ev.type)
            {
                case SDL_QUIT:
                rs->runing=0;
                goto exit_loop;
                case SDL_KEYUP:
                {
                    action_func action;
                    if(map_get(keymap,ev.key.keysym.sym,(void **)&action)){
                        continue;
                    }
                    ERR_EXIT(action(NULL));
                }
                break;
                case SDL_WINDOWEVENT:
                if(ev.window.event==SDL_WINDOWEVENT_EXPOSED){
    //                SDL_RenderPresent(rs->ren);
                    rs->present=1;
                // } else {
                //     SDL_Log("win event:%04x,type: %04x,data1:%d,data2:%d",ev.window.event,ev.window.type,ev.window.data1,ev.window.data2);
                }
                break;
                default:
                //SDL_Log("unhandle event:%04x",ev.type);
                break;
            }
        }
        if(rs->present){
            Uint32 now=SDL_GetTicks();
            if(now-tick> FP){
                rs->present=0;
                // present函数不能调用得太频繁，否则会出现渲染错误
                SDL_RenderPresent(rs->ren);
                tick=now;
            }
        }
    }
    exit_loop:
    map_free(keymap);
    return 0;
    err_exit:
    map_free(keymap);
    return -1;
}