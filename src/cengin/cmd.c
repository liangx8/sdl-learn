#include <malloc.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_log.h>
#include "queue.h"

typedef int (*Command)(void *);


typedef struct
{
    SDL_Thread *runThread;
    SDL_mutex  *mutex;
    SDL_cond   *cond;
    QUEUE      *queue;
    int         runing;
} THREAD_CMD;
int thread_run(void *param)
{
    THREAD_CMD *cmds=(THREAD_CMD *)param;
    SDL_LockMutex(cmds->mutex);
    while(cmds->runing){
        void *arg;
        Command cmd;
        SDL_CondWait(cmds->cond,cmds->mutex);
        while(queue_pop_front(cmds->queue,(void **)&cmd,&arg)==0){
            SDL_UnlockMutex(cmds->mutex);
            cmd(arg);
            SDL_LockMutex(cmds->mutex);
        }
    }
    SDL_UnlockMutex(cmds->mutex);
    return 0;
}
int cmds_close(THREAD_CMD *cmds)
{
    cmds->runing=0;
    SDL_LockMutex(cmds->mutex);
    SDL_CondSignal(cmds->cond);
    SDL_UnlockMutex(cmds->mutex);
    SDL_WaitThread(cmds->runThread,NULL);
    free(cmds->queue);
    SDL_DestroyCond(cmds->cond);
    SDL_DestroyMutex(cmds->mutex);
    return 0;
}
int cmds_init(THREAD_CMD *tcmd)
{
    tcmd->runing=1;
    if((tcmd->mutex=SDL_CreateMutex())==NULL){
        SDL_LogError(0,"create mutex error %s",SDL_GetError());
        return -1;
    }
    if((tcmd->cond=SDL_CreateCond())==NULL){
        SDL_LogError(0,"create cond error %s",SDL_GetError());
        goto ret_err1;
    }
    tcmd->queue=queue_new(20);
    if((tcmd->runThread=SDL_CreateThread(thread_run,"exec command",tcmd))==NULL){
        SDL_LogError(0,"create thread error %s",SDL_GetError());
        goto ret_err2;
    }
    return 0;

ret_err2:
    free(tcmd->queue);
    SDL_DestroyCond(tcmd->cond);
ret_err1:
    SDL_DestroyMutex(tcmd->mutex);
    return -1;
}
int cmds_put(THREAD_CMD *cmds,Command cmd,void *arg)
{
    while(1){
        if(cmds->runing){
            SDL_LockMutex(cmds->mutex);
            int res=queue_push_back(cmds->queue,cmd,arg);
            SDL_UnlockMutex(cmds->mutex);
            if(res){
                continue;
            }
            SDL_CondSignal(cmds->cond);
            break;
        } else {
            return -1;
        }
    }
    return 0;
}