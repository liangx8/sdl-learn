#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "cengin/ce_app.h"
#include "resource.h"
#define BORDER 100
const char *random_file(long *pidx);

void setPicture(CE_APP *app,long *pidx)
{
    const char *fname=random_file(pidx);
    if(fname==NULL){
        *pidx=0;
        return;
    }
    SDL_Surface *surf;
    surf=IMG_Load(fname);
    if(surf==NULL){
        SDL_Log("Load file %s\n",IMG_GetError());
    } else {
        SDL_Log("width: %3d, height: %3d:filename:%s\n",surf->w,surf->h,fname);
        ce_blit_surface(app,surf,NULL,NULL);
        SDL_FreeSurface(surf);
    }
}
int ea(CE_APP *app,SDL_Event *ev)
{
    RESOURCES *res=(RESOURCES*)app->payload;
    if(ev->type==SDL_KEYUP){
        switch (ev->key.keysym.sym){
            case 'n':
                setPicture(app,(long *)app->payload);
                return 0;
            case 'f':
                //SDL_RenderCopy(app->render,res->background,NULL,NULL);
                SDL_RenderPresent(app->render);
                return 1;
            case 'r':
                ce_show_text(app,&res->tfont,"Hello 2024",20,100);
                SDL_RenderPresent(app->render);
            case SDLK_UP:
            case SDLK_DOWN:
            case SDLK_LEFT:
            case SDLK_RIGHT:
            return 2;
        }
    }
    return 0;
}
int cmd_sample(void *);
int menu_set(CE_APP *app);
int load_resources(CE_APP *app,RESOURCES *res);
void free_res(RESOURCES *res);
int menu_event(CE_APP *app,SDL_Event *ev);
void game_init(int rows,int cols);
void sdl_sample()
{
    RESOURCES res;
    res.val=0;
    CE_APP *app=ce_init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK);
    if(app==NULL){
        return;
    }
    app->payload=&res;
    if(cmds_init(&app->cmds)){
        return;
    }

    const int win_w=app->dm.w-BORDER*8;
    const int win_h=app->dm.h-BORDER-BORDER;
    if(ce_create_window(app,"方块消除",win_w,win_h,SDL_WINDOW_SHOWN)){
        return;
    }
    SDL_Log("建立窗口[宽:%d 高:%d]",win_w,win_h);
    const int img_flag= IMG_INIT_PNG;
    if(!(IMG_Init(img_flag) & img_flag)){
        SDL_LogError(0,"Image initial failure with error %s\n",IMG_GetError());
        goto error_exit1;
    }
    if(TTF_Init()){
        SDL_LogError(0,"TTF initial failure with error %s\n",TTF_GetError());
        goto error_exit2;
    }

    if(load_resources(app,&res)){
        SDL_Log("load resources error");
        goto error_exit2;
    }
    if(SDL_RenderCopy(app->render,res.background,NULL,NULL)){
        SDL_Log("RenderCopy() %s",SDL_GetError());
        goto error_exit3;
    }
    game_init(46,35);
    menu_set(app);
    setPicture(app,(long *)&res);
    SDL_RenderPresent(app->render);
    ce_run(app);
    cmds_close(&app->cmds);
error_exit3:
    free_res(&res);
error_exit2:
    IMG_Quit();
error_exit1:
    ce_cleanup(app);
}
