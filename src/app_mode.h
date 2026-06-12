#ifndef ABSTRACT_MODE_H
#define ABSTRACT_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <SDL2/SDL.h>
/* Abstract mode interface for SDL-based applications. */

typedef struct _APP_MODE APP_MODE;

typedef struct AbstractModeVTable {
    void* (*init)(APP_MODE *mode);
    int (*event)(APP_MODE *mode, SDL_Event *event,void *data);
    int (*render)(APP_MODE *mode,void *data);
    int (*destroy)(APP_MODE *mode, void *data);
} AbstractModeVTable;
struct MODE_BIND{
    const AbstractModeVTable *vtable;
    void *data;
};
struct _APP_MODE {
    struct MODE_BIND *mode_bind;
    SDL_Window *window;
    SDL_Renderer *renderer;
};


#ifdef __cplusplus
}
#endif

#endif /* ABSTRACT_MODE_H */
