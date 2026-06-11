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
    void (*init)(APP_MODE *mode);
    void (*event)(APP_MODE *mode, SDL_Event *event);
    void (*render)(APP_MODE *mode);
    void (*shutdown)(APP_MODE *mode);
} AbstractModeVTable;

struct _APP_MODE {
    const AbstractModeVTable *vtable;
    SDL_Window *window;
    SDL_Renderer *renderer;
    void *userdata;
};


#ifdef __cplusplus
}
#endif

#endif /* ABSTRACT_MODE_H */
