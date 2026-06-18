#ifndef ABSTRACT_MODE_H
#define ABSTRACT_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <SDL2/SDL.h>
typedef struct _sdl_app SdlApp;

/* Abstract mode interface for SDL-based applications. */
typedef struct AbstractModeVTable {
    // void* (*init)(SdlApp *stage);

    // event handler
    int (*event)(SdlApp *mode, SDL_Event *event,void *data);
    int (*render)(SdlApp *mode,void *data);
    int (*destroy)(SdlApp *mode, void *data);
    int (*pause)(SdlApp *mode, void *data);
    int (*resume)(SdlApp *mode, void *data);
} AbstractModeVTable;
typedef struct _stage {
    AbstractModeVTable *action;
    void *stage_data;
} STAGE;

#ifdef __cplusplus
}
#endif

#endif /* ABSTRACT_MODE_H */
