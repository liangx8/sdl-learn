#include <SDL2/SDL.h>
#include "cengin/ce_app.h"
#define ITEM_COUNT 4
typedef struct {
    long val;
    SDL_Texture  *background;
    TEXTURE_FONT  tfont;
    // 菜单所有项全渲染到一张纹理(Texture)上
    SDL_Texture  *menuitem;
    SDL_Rect      range[ITEM_COUNT*2];
    int           menuSel;
    int           oldMenuSel;
} RESOURCES;
