#pragma once
#include "map.h"
#include "abc_action.h"

typedef struct {
    action_func attach;
    typeof (int (*)(SDL_Event *)) userEvent;
    action_func dettech;
} STAGE_ACTION;
typedef struct _STAGE{
    const STAGE_ACTION *action;
    void *payload;
} STAGE;

//
#define LOCK_MUTEX() do{}while(0)
#define UNLOCK_MUTEX() do{}while(0)