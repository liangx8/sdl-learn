#include <malloc.h>
#include <strings.h>
#include "cengin/ce_app.h"
#include "resource.h"
#define CELL_SIZE 20
#define BLOCKS_MAX_SIZE 16

//void print_rect(SDL_Rect *);
//void print_array(void *,int size);
typedef Uint16 BLOCKS;
const BLOCKS sqr1=0b1;
const BLOCKS bar2=0b11;
const BLOCKS bar3=0b111;
const BLOCKS bar4=0b1111;

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
const BLOCKS barray[]={sqr1,bar2,bar3,bar4,bar5,bar8,z1,z2,longL,c3,c4,square};
/// @brief 预定义的颜色索引[1~255]
Uint32 colors[256];

typedef struct{
    int       rows,cols;
    /// @brief  占据的窗口位置
    SDL_Rect  area;
    /// @brief 保存的是在matrix中的位置索引
    Uint16 block[BLOCKS_MAX_SIZE];
    /// @brief block的颜色索引
    int block_color_idx;
    /// @brief 保存的是颜色索引或者０是空位
    char      matrix[];
} GAME;
GAME *g_game;
// g_preview.rows　必须被设为0
GAME g_preview;
BLOCKS g_next_blocks;
SDL_TimerID g_timer_id;
int outdate;
Uint16 g_oldblock[BLOCKS_MAX_SIZE];
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
//void print_square(Uint8 *data);
/**
 * @brief 旋转GAME::block
 * @return 返回非零，如果旋转不成功，返回０
 */
int game_rotate(GAME *game)
{
    /*只进行90度的旋转, 先把数据放到一个较小的正方形阵列。转动后，在对齐取出来*/
    const int WIDTH=8;
    const int area_w=game->cols;
    Uint8 square[WIDTH*WIDTH];
    bzero(square,WIDTH*WIDTH);
    int mostx=area_w;
    int mosty=game->rows;
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        int cur;
        if((cur=game->block[ix])==0)break;
        cur --;
        int stx=cur % area_w;
        if(stx < mostx){
            mostx=stx;
        }
        int sty=cur / area_w;
        if(sty< mosty){
            mosty=sty;
        }
    }
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        int cur;
        if((cur=game->block[ix])==0)break;
        cur --;
        int x=cur % area_w - mostx;
        int y=cur / area_w - mosty;
        square[y*WIDTH+x]=1;
    }
    for(int ix=0;ix < WIDTH;ix++){
        for(int iy=0;iy<WIDTH/2;iy++){
            Uint8 tmp=square[iy*WIDTH+ix];
            square[iy*WIDTH+ix]=square[(WIDTH-iy-1)*WIDTH+ix];
            square[(WIDTH-iy-1)*WIDTH+ix]=tmp;
        }
    }
    int xmax=WIDTH;
    int ymax=WIDTH;
    for(int ix=0;ix < WIDTH;ix++){
        for(int iy=ix+1;iy<WIDTH;iy++){
            Uint8 tmp1=square[ix*WIDTH + iy];
            Uint8 tmp2=square[iy*WIDTH + ix];
            square[ix*WIDTH + iy]=tmp2;
            square[iy*WIDTH + ix]=tmp1;
            if(tmp1){
                if(iy < ymax)ymax=iy;
                if(ix < xmax)xmax=ix;
            }
            if(tmp2){
                if(ix<ymax)ymax=ix;
                if(iy<xmax)xmax=iy;
            }
        }
    }
    int idx=0;
    for(int iy=ymax;iy < WIDTH;iy++){
        for(int ix=xmax;ix<WIDTH;ix++){
            if(square[iy*WIDTH+ix]){
                int x=mostx+ix-xmax;
                if(x<0)return 0;
                int y=mosty+iy-ymax;
                if(y<0)return 0;
//                game->block[idx]=x + y*area_w+1;
                idx ++;
            }
        }
    }
    idx=0;
    for(int iy=ymax;iy < WIDTH;iy++){
        for(int ix=xmax;ix<WIDTH;ix++){
            if(square[iy*WIDTH+ix]){
                int x=mostx+ix-xmax;
                int y=mosty+iy-ymax;
                game->block[idx]=x + y*area_w+1;
                idx ++;
            }
        }
    }
    return -1;
}
int game_down(GAME *game)
{
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        if(game->block[ix]==0)break;
        int cur=game->block[ix]-1;
        int newy=cur+game->cols;
        if(newy>= game->cols*game->rows){
            return 0;
        }
        if(game->matrix[newy]){
            // 当前格已经有内容不能进入
            return 0;
        }
    }
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        if(game->block[ix]==0)break;
        game->block[ix]=game->block[ix]+game->cols;
    }
    
    return -1;
}
int game_left(GAME *game)
{
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        if(game->block[ix]==0)break;
        int cur=game->block[ix]-1;
        int low=(cur / game->cols) * game->cols;
        if(cur == low){
            return 0;
        }
        cur --;
        
        if(game->matrix[cur]){
            // 当前格已经有内容不能进入
            return 0;
        }
    }
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        if(game->block[ix]==0)break;
        game->block[ix]=game->block[ix]-1;
    }
    
    return -1;
}

int game_right(GAME *game)
{
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        if(game->block[ix]==0)break;
        int cur=game->block[ix]-1;
        int roof=(cur / game->cols + 1) * game->cols;
        if(cur == (roof-1)){
            return 0;
        }

        cur ++;
        if(game->matrix[cur]){
            // 当前格已经有内容不能进入
            return 0;
        }
    }
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        if(game->block[ix]==0)break;
        game->block[ix]=game->block[ix]+1;
    }
    return -1;
}
void game_fall(GAME *game)
{
    int loop=1;
    int grd=game->rows;
    while(loop){
        int saft=1;
        for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
            int cur=game->block[ix];
            if(cur==0){
                break;
            }
            cur --;
            int x=cur % game->cols;
            int y=cur / game->rows;
            int idx=(grd+y) * game->cols + x;
            if(idx >= game->cols * game->rows){
                grd--;
                saft=0;
                break;
            }
            if(game->matrix[idx]){
                grd--;
                saft=0;
                break;
            }
        }
        if(saft){
            for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
                int cur=game->block[ix];
                if(cur==0){
                    break;
                }
                cur --;
                int x=cur % game->cols;
                int y=cur / game->rows;
                game->block[ix]=(grd+y) * game->cols + x+1;
            }
            loop=0;
        }
    }
}
void game_land(GAME *game)
{
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        if(game->block[ix]==0)break;
        Uint16 idx=game->block[ix]-1;
        game->block[ix]=0;
        game->matrix[idx]=game->block_color_idx;
    }
}
/**
 * @brief 消行
 * 
 */
void game_coll(GAME *game)
{
    for(int iy=game->rows-1;iy>=0;iy--){
        int complete=1;
        for(int ix=0;ix<game->cols;ix++){
            if(game->matrix[iy*game->cols+ix]==0){
                complete=0;
                break;
            }
        }
        if(complete){
            for(int ix=0;ix<game->cols;ix++){
                game->matrix[iy*game->cols+ix]=0;
            }
        }
    }
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
    for(int idx=0;idx<game->cols * game->rows;idx++){
        colorIdx=game->matrix[idx];
        if(colorIdx){
            Uint32 color=colors[colorIdx & 0xff];
            const Uint8 a= color & 0xff;
            const Uint8 b= (color >> 8) & 0xff;
            const Uint8 g= (color >> 16) & 0xff;
            const Uint8 r= color >> 24;
            HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_SetRenderDrawColor(app->render,r,g,b,a))
            int x=idx % game->cols;
            int y=idx / game->cols;
            box.x=stx + 1 + x * (CELL_SIZE+1);
            box.y=sty + 1 + y * (CELL_SIZE+1);
            HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderFillRect(app->render,&box));
        }
    }
    return 0;
}
int game_cmd(void *pl)
{
    GAME *game=(GAME*)pl;
    
    if(game_down(game)==0){
        game_land(game);
        HANDLER_NOT_ZERO_RET_NEG(game_populate_data(g_game,g_next_blocks,g_preview.block_color_idx))
        g_next_blocks=barray[rand() %12];
        HANDLER_NOT_ZERO_RET_NEG(game_populate_data(&g_preview,g_next_blocks,rand()))
    }
    outdate=1;
    return 0;
}
Uint32 game_timer_run(Uint32 interval,void *param)
{
    CE_APP *app=(CE_APP*)param;
    cmds_put(&app->cmds,game_cmd,g_game);
    return 8000;
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

#define AREA_R 0x3e
#define AREA_G 0x66
#define AREA_B 0x5a

int game_oldblock_clear(CE_APP *app)
{
    int stx=g_game->area.x;
    int sty=g_game->area.y;

    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_SetRenderDrawColor(app->render,AREA_R,AREA_G,AREA_B,0xff))
    SDL_Rect box;
    box.w=CELL_SIZE;
    box.h=CELL_SIZE;
    for(int ix=0;ix<BLOCKS_MAX_SIZE;ix++){
        Uint16 idx=g_oldblock[ix];
        if(idx == 0)break;
        idx --;
        int x=idx % g_game->cols;
        int y=idx / g_game->cols;
        box.x=stx + 1 + x * (CELL_SIZE+1);
        box.y=sty + 1 + y * (CELL_SIZE+1);
        HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderFillRect(app->render,&box));
        //print_rect(&box);
    }
    bcopy((void *)g_game->block,(void *)g_oldblock,BLOCKS_MAX_SIZE * sizeof(BLOCKS));
    return 0;
}

int game_present(CE_APP *app)
{
    if(outdate){
        outdate=0;
        HANDLER_NOT_ZERO_RET_NEG(game_oldblock_clear(app))
        HANDLER_NOT_ZERO_RET_NEG(game_draw(app,g_game))
        HANDLER_NOT_ZERO_RET_NEG(game_draw(app,&g_preview))
        SDL_RenderPresent(app->render);
    }
    return 0;
}
int menu_set(CE_APP *app);
int game_event(CE_APP *app,SDL_Event *ev)
{
    RESOURCES *res=(RESOURCES*)app->payload;
    if(ev->type==SDL_KEYUP){
        switch(ev->key.keysym.sym){
            case SDLK_ESCAPE:
                HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderCopy(app->render,res->background,&g_game->area,&g_game->area));
                HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderCopy(app->render,res->background,&g_preview.area,&g_preview.area));
                SDL_RemoveTimer(g_timer_id);
                return menu_set(app);
            case SDLK_UP:
                if(game_rotate(g_game)){
                    outdate=1;
                }
                break;
            case SDLK_LEFT:
                if(game_left(g_game)){
                    outdate=1;
                }
                break;
            case SDLK_RIGHT:
                if(game_right(g_game)){
                    outdate=1;
                }
                break;
            case SDLK_DOWN:
                if(game_down(g_game)){
                    outdate=1;
                }
                break;
            case SDLK_SPACE:
                game_fall(g_game);
                game_land(g_game);
                HANDLER_NOT_ZERO_RET_NEG(game_populate_data(g_game,g_next_blocks,g_preview.block_color_idx))
                g_next_blocks=barray[rand() %12];
                HANDLER_NOT_ZERO_RET_NEG(game_populate_data(&g_preview,g_next_blocks,rand()))
                outdate=1;
                break;
        }
    }
    
    return 0;
}
//void print_rect(SDL_Rect *);

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
    //backgound color
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_SetRenderDrawColor(app->render,AREA_R,AREA_G,AREA_B,0xff));
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderFillRect(app->render,&game->area));
    // outer border color
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_SetRenderDrawColor(app->render,0x0,0xd3,0xb0,0xff));
    HANDLER_NOT_ZERO_SDLERR_RET_NEG(SDL_RenderDrawRect(app->render,&game->area));
    // inner border color
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
    game_clear(g_game);
    bcopy((void *)g_game->block,(void *)g_oldblock,BLOCKS_MAX_SIZE * sizeof(BLOCKS));
    HANDLER_NOT_ZERO_RET_NEG(game_draw(app,&g_preview))
    HANDLER_NOT_ZERO_RET_NEG(game_draw(app,g_game))
    SDL_RenderPresent(app->render);
    app->present=game_present;
    app->on_event=game_event;
    if((g_timer_id=SDL_AddTimer(3000,game_timer_run,app))==0){
        SDL_LogError(0,"create timer error%s",SDL_GetError());
        return -1;
    }
    return 0;
}
