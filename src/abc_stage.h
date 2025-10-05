#pragma once
#include "map.h"
#include "abc_action.h"

typedef struct {
    action_func attach;
    action_func run;
    action_func dettech;
} STAGE_ACTION;
typedef struct {
    const STAGE_ACTION *action;
    void *payload;
} STAGE;

