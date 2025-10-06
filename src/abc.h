#pragma once

#include "abc_runstate.h"
#include "abc_action.h"
#include "abc_stage.h"

#define MINUS_ERR(x) if(x){return -1;}
#define NULL_ERR(x) if(x){return NULL;}
int app_run(RUNSTATE *rs,STAGE *start);
int switch_stage(STAGE *);
#define USER_TYPE_MAX 2
/*
 *自定义类型需要用SDL_RegisterEvents()来告知系统，正常来说会从0x8000开始
 */
enum ABC_USER_EVENT_TYPE{
    USER1_TYPE = 0x8000,
    USER2_TYPE
};

