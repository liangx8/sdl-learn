#include <malloc.h>
#include <strings.h>
#include "cengin/ce_app.h"
#include "resource.h"
#define CELL_SIZE 20
#define BLOCKS_MAX_SIZE 16

//void print_rect(SDL_Rect *);

typedef Uint16 BLOCKS;
/**
 * -----
 */
const BLOCKS bar5=0b11111;
const BLOCKS bar8=0b11111111;
/**
 * -+
 *  +--
 */
const BLOCKS z1=0b11+(0b00001110<<8);
/**
 * -+
 *  +--
 */
const BLOCKS z2    = 0b111+(0b11100<<8);
const BLOCKS longL = 0b111111111;
const BLOCKS c3    = 0b111+(0b0101<<8);
const BLOCKS c4    = 0b1111+(0b1001<<8);
const BLOCKS square= 0b11+(0b11 << 8);
const BLOCKS barray[]={bar5,bar8,z1,z2,longL,c3,c4,square};
Uint32 colors[256];

typedef struct{
    int       rows,cols;
    /// @brief  占据的窗口位置
    SDL_Rect  area;
    Uint16 block[BLOCKS_MAX_SIZE];
    int block_color_idx;
    char      matrix[];
} GAME;
GAME *g_game;
// g_preview.rows　必须被设为0
GAME g_preview;
BLOCKS g_next_blocks;
int game_populate_data(GAME *game,const BLOCKS blocks,int colorIdx)
{
    // 期望BLOCKS_DATA::game已经被指定。
    if(game==NULL){
        SDL_LogError(0,"期望这里有数值");
        return -1;
    }
    game->block_color_idx=colorIdx;
    bzero(game->block,2*BLOCKS_MAX_SIZE);
    int ia=0;
    int stx=(game->cols - BLOCKS_MAX_SIZE/2)/2;
    for(int seq=0;seq <BLOCKS_MAX_SIZE;seq++){
        int iy=seq / 8;
        int ix=seq % 8;
        if((blocks >> seq) & 1){
            game->block[ia]=stx+ix+iy * game->cols + 1;
            ia++;
        }
    }
    
    return 0;
}

int game_rotate(GAME *game)
{
    return 0;
}
int game_down(GAME *game)
{
    return 0;
}
int game_left(GAME *game)
{
    return 0;
}
int game_right(GAME *game)
{
    return 0;
}


int game_draw(CE_APP *app,GAME *game)
{
    int stx=game->area.x;
    int sty=game->area.y;
    int colorIdx=game->block_color_idx;
    Uint32 color=colors[colorIdx & 0xff];
    const Uint8 a= color & 0xff;
    const Uint8 b= (color >> 8) & 0xff;
    const Uint8 g= (color >> 16) & 0xff;
    const Uint8 r= color >> 24;
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_SetRenderDrawColor(app->render,r,g,b,a))
    SDL_Rect box;
    box.w=CELL_SIZE;
    box.h=CELL_SIZE;
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        Uint16 idx=game->block[ix];
        if(idx == 0)break;
        idx --;
        int x=idx % game->cols;
        int y=idx / game->cols;
        box.x=stx + 1 + x * (CELL_SIZE+1);
        box.y=sty + 1 + y * (CELL_SIZE+1);
        HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderFillRect(app->render,&box));
        //print_rect(&box);
    }
    for(int ix=0;ix<game->cols * game->rows;ix++){

    }
    return 0;
}

void game_init(int rows,int cols)
{
    const int offset=sizeof(GAME);
    g_game=malloc(offset+rows*cols);
    g_game->rows=rows;
    g_game->cols=cols;
    for(int ix=0;ix<256;ix++){
        colors[ix]=rand() | 0xff;
    }

}
void game_clear(GAME *game)
{
    bzero(&game->matrix[0],game->cols * game->rows);
}

int game_present(CE_APP *app)
{
    return 0;
}




int menu_set(CE_APP *app);
int game_event(CE_APP *app,SDL_Event *ev)
{
    RESOURCES *res=(RESOURCES*)app->payload;
    if((ev->type=SDL_KEYUP) && (ev->key.keysym.sym == SDLK_ESCAPE)){
        HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderCopy(app->render,res->background,&g_game->area,&g_game->area));
        HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderCopy(app->render,res->background,&g_preview.area,&g_preview.area));
        return menu_set(app);
    }
    
    return 0;
}
//void print_rect(SDL_Rect *);

#define AREA_R 0x3e
#define AREA_G 0x66
#define AREA_B 0x5a
int game_area(CE_APP *app,GAME *game,int x,int y)
{
    const int width = game->cols * (CELL_SIZE+1)+1;
    int rows=game->rows;
    if(rows==0){
        rows=2;
    }
    const int height=rows * (CELL_SIZE+1)+1;
    if(y==0){
        y = (app->win_h-height)/2;
    }

    game->area.x=x;
    game->area.y=y;
    game->area.w=width;
    game->area.h=height;
    //print_rect(&game->area);
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_SetRenderDrawColor(app->render,AREA_R,AREA_G,AREA_B,0xff));
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderFillRect(app->render,&game->area));
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_SetRenderDrawColor(app->render,0x0,0xd3,0xb0,0xff));
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderDrawRect(app->render,&game->area));
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_SetRenderDrawColor(app->render,AREA_R-0x10,AREA_G-0x10,AREA_B-0x10,0));

#if 1
    for(int ix=0;ix<rows-1;ix++){
        int ya=y+(ix+1)*(CELL_SIZE+1);
        HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderDrawLine(app->render,x+1,ya,x+width-2,ya));
    }
    for(int ix=0;ix<game->cols-1;ix++){
        int xa=x+(ix+1)*(CELL_SIZE+1);
        HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderDrawLine(app->render,xa,y+1,xa,y+height-2));
    }
#else
    for(int ix=0;ix<rows-1;ix++){
        int ya=y+(ix+1)*(CELL_SIZE+1);
        HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderDrawLine(app->render,x+1,ya,x+width-2,ya));
        SDL_Log("y line:%d",ya);
    }
    for(int ix=0;ix<game->cols-1;ix++){
        int xa=x+(ix+1)*(CELL_SIZE+1);
        HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderDrawLine(app->render,xa,y+1,xa,y+height-2));
        SDL_Log("x line:%d",xa);
    }
    //SDL_Log("area(width:%d,height:%d),rows:%d,cols:%d",width,height,game->rows,game->cols);
#endif
    return 0;
}

int game_set(CE_APP *app)
{
    HANDLER_NOT_ZERO_RET_NEG(game_area(app,g_game,25,0))
    g_preview.cols=8;
    // 让g_preview.matrix中没有内容。
    g_preview.rows=0;
    HANDLER_NOT_ZERO_RET_NEG(game_area(app,&g_preview,850,80))
    g_next_blocks=barray[rand() & 0xf];
    HANDLER_NOT_ZERO_RET_NEG(game_populate_data(&g_preview,g_next_blocks,rand()))
    HANDLER_NOT_ZERO_RET_NEG(game_populate_data(g_game,barray[rand() & 0xf],rand()))
    HANDLER_NOT_ZERO_RET_NEG(game_draw(app,&g_preview))
    HANDLER_NOT_ZERO_RET_NEG(game_draw(app,g_game))
    SDL_RenderPresent(app->render);
    app->present=game_present;
    app->on_event=game_event;
    return 0;
}
