#pragma once
#include "map.h"
#include "abc_action.h"

typedef struct {
    int (*attach)(RUNSTATE *,MAP,void *);
    action_func run;
    action_func dettech;
} STAGE_ACTION;
typedef struct {
    const STAGE_ACTION *action;
    void *payload;
} STAGE;

