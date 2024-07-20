#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "ce_app.h"

int ce_texture_font(CE_APP *app,TEXTURE_FONT *tFont, TTF_Font *font,SDL_Color color)
{
    int total=0x7f-0x21;
    char chs[total+1];
    for(int ix=0x21;ix<127;ix++){
        chs[ix-0x21]=ix;
    }
    chs[total]='\0';
    SDL_Log("[%s]",&chs[0]);
    SDL_Texture *tx=ce_texture_create_from_text(app,font,&chs[0],color);
    if(tx==NULL){
        return -1;
    }
    tFont->font=tx;
    int w;
    SDL_QueryTexture(tx,NULL,NULL,&w,&tFont->fontHeight);
    tFont->fontWidth=w/94;
    SDL_Log("font size(width:%d,height:%d)",tFont->fontWidth,tFont->fontHeight);

    return 0;
}
int ce_show_text(CE_APP *app,TEXTURE_FONT *tFont,const char *msg,int x,int y)
{
    
    const char *ptr=msg;
    SDL_Rect srcrect={0,0,tFont->fontWidth,tFont->fontHeight};
    SDL_Rect dstrect={x,y,tFont->fontWidth,tFont->fontHeight};
    while(*ptr){
        int idx=*ptr - 0x21;
        srcrect.x=idx * tFont->fontWidth;
        SDL_RenderCopy(app->render,tFont->font,&srcrect,&dstrect);
        dstrect.x += tFont->fontWidth;
        ptr ++;
    }
    return 0;
}