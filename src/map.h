#pragma once

typedef void* MAP;
MAP map_new(int);
#define map_free(x) free(x)

int map_set(MAP,int,const void*);
int map_get(MAP,int,void**);
void map_clear(MAP);
