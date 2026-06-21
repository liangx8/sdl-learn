#ifndef ABSTRACT_MODE_H
#define ABSTRACT_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <SDL2/SDL.h>
typedef struct _SdlApp SdlApp;

/* Abstract mode interface for SDL-based applications. */
typedef struct _AbstractActionVTable {
    int (*init)(SdlApp *mode);

    // event handler
    int (*event)(SdlApp *mode, SDL_Event *event);
    int (*render)(SdlApp *mode);
    int (*destroy)(SdlApp *mode);
    int (*pause)(SdlApp *mode);
    int (*resume)(SdlApp *mode);
} AbstractActionVTable;

#ifdef __cplusplus
}
#endif

#endif /* ABSTRACT_MODE_H */
