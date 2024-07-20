#include <SDL2/SDL.h>
#include "cengin/ce_app.h"
typedef struct {
    long val;
    SDL_Texture *background;
    TEXTURE_FONT tfont;
    // 菜单所有项全渲染到一张纹理(Texture)上
    SDL_Texture *menuitem;
} RESOURCES;
