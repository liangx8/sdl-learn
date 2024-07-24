#include "cengin/ce_app.h"
#include "resource.h"
#define MENU_WIDTH 600
int menu_update;
int lowlight(CE_APP *app)
{
    RESOURCES *res=app->payload;
    SDL_Rect *sr=&res->range[res->oldMenuSel+ITEM_COUNT];
    const int x1=(app->win_w-MENU_WIDTH)/2;
    const int y1=sr->y-2;
    const int x2=x1+MENU_WIDTH-1;
    const int y2=y1 + sr->h+4-1;
    SDL_Rect dst={x1,y1,MENU_WIDTH,1};
    if(SDL_RenderCopy(app->render,res->background,&dst,&dst)){
        SDL_LogError(0,"lowlight():1 %s",SDL_GetError());
        return -1;
    }
    dst.w=1;
    dst.h=sr->h+4;
    if(SDL_RenderCopy(app->render,res->background,&dst,&dst)){
        SDL_LogError(0,"lowlight():2 %s",SDL_GetError());
        return -1;
    }
    dst.y=y2;
    dst.w=MENU_WIDTH;
    dst.h=1;
    if(SDL_RenderCopy(app->render,res->background,&dst,&dst)){
        SDL_LogError(0,"lowlight():3 %s",SDL_GetError());
        return -1;
    }
    dst.x=x2;
    dst.w=1;
    dst.y=y1;
    dst.h=sr->h+4;
    if(SDL_RenderCopy(app->render,res->background,&dst,&dst)){
        SDL_LogError(0,"lowlight():4 %s",SDL_GetError());
        return -1;
    }
    res->oldMenuSel=res->menuSel;
    return 0;
}
int highlight(CE_APP *app)
{
    RESOURCES *res=app->payload;
    SDL_SetRenderDrawColor(app->render,0xf7,0xbe,0x05,0xff);
    SDL_Rect *sr=&res->range[res->menuSel+ITEM_COUNT];
    SDL_Rect rect={(app->win_w-MENU_WIDTH)/2,sr->y-2,MENU_WIDTH,sr->h+4};

    if(SDL_RenderDrawRect(app->render,&rect)){
        SDL_LogError(0,"%s",SDL_GetError());
        return -1;
    }
    if(res->menuSel!=res->oldMenuSel){
        lowlight(app);
    }
    
    return 0;
}
int menu_present(CE_APP *app)
{
    if(menu_update){
        menu_update=0;
        highlight(app);
        SDL_RenderPresent(app->render);
    }
    return 0;
}
int game_set(CE_APP *app);
int menu_event(CE_APP *app,SDL_Event *ev)
{
    RESOURCES *res=app->payload;
    if(ev->type == SDL_KEYUP){
        switch (ev->key.keysym.sym){
            case SDLK_UP:
            {
                int sel=res->menuSel-1;
                if(sel == -1){
                    sel=ITEM_COUNT -1;
                }
                res->menuSel=sel;
                menu_update=1;
                break;
            }

            case SDLK_DOWN:
            {
                int sel=res->menuSel+1;
                if (sel == ITEM_COUNT){
                    sel=0;
                }
                res->menuSel=sel;
                menu_update=1;
                break;
            }
            case SDLK_RETURN:
                if(res->menuSel==0){
                    game_set(app);
                }
                if(res->menuSel==3){
                    return -1;
                }

        }
    }
    return 0;
}
int menu_set(CE_APP *app)
{
    RESOURCES *res=app->payload;
    for(int ix=0;ix<ITEM_COUNT;ix++){
        SDL_Rect *src=&res->range[ix];
        SDL_Rect *dst=&res->range[ix+ITEM_COUNT];
        if(SDL_RenderCopy(app->render,res->menuitem,src,dst)){
            SDL_LogError(0,"%s",SDL_GetError());
            return -1;
        }
    }
    if(highlight(app)){
        return -1;
    }
    app->on_event=menu_event;
    app->present=menu_present;
    SDL_RenderPresent(app->render);
    return 0;
}
