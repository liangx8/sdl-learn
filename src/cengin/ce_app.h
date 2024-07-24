#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define RGBA8888(r,g,b,a) ((a) | ((b)<<8) | ((g)<<16) | ((r)<<24))



// advance define
struct _CE_APP;


typedef int (*EventAction)(struct _CE_APP *,SDL_Event *);
typedef int (*PresentAction)(struct _CE_APP *);

typedef struct {
    char padding[64];
} THREAD_CMD;


struct _CE_APP{
    SDL_Window      *win;
    int             win_w,win_h;
    SDL_Renderer    *render;
    SDL_DisplayMode dm;
    // on_event() 返回任何非０即视为程序要退出
    EventAction     on_event;
    PresentAction   present;
    THREAD_CMD      cmds;
    void            *payload;
};
typedef struct _CE_APP CE_APP;
typedef struct {
    SDL_Texture *font;
    int fontWidth;
    int fontHeight;
} TEXTURE_FONT;

/**
 * @brief 创建texture时，选参数ACCESSTARGET,用api构建内容使用的回调函数
 */
typedef int (*PaintTexture)(SDL_Renderer *,int ,int ,void *);
/**
 * @brief 创建texture时，选参数ACCESSTREAM,用pixels构建内容使用的回调函数
 */
typedef int (*PaintPixels)(void *pixel,int pitch,int height,void *param);

CE_APP* ce_init (int flags);
int     ce_create_window(CE_APP *app,const char *title,int width,int height,int flags);
void    ce_cleanup(CE_APP *app);
int     ce_blit_surface(CE_APP *app,SDL_Surface *surf,SDL_Rect *src,SDL_Rect *dst);
/**
 * @brief 把texture不变形渲染到屏幕的指定位置上
 */
int ce_render_copy(CE_APP *,SDL_Texture *texture, int x,int y);
/**
 * @brief 运行ＳＤＬ应用，事件触发EventAction
 * @param app application
 */
int ce_run(CE_APP *app);
////////////////////////////////////////////////////////////////////////////////////////
/// 实现源码 ce_texture.c
////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief 用图像建立TEXTURE
 */
SDL_Texture *ce_texture_create_from_image(CE_APP *app,const char *image);
/**
 * @brief 用文字建立TEXTURE
 */
SDL_Texture *ce_texture_create_from_text(CE_APP *app,TTF_Font *font,const char* text,SDL_Color color);
/**
 * @brief 建立空白TEXTURE,并用API画内容上去
 */
SDL_Texture *ce_texture_create_target(CE_APP *app,PaintTexture pt,int w,int h,void *param);
/**
 * @brief 画TEXTURE中pixels数据直接画, 建立texture 时，必须带参数SDL_TEXTUREACCESS_STREAMING, 用ce_texture_create_target更直观点
 */
int ce_texture_paint_pixels(SDL_Texture *texture,PaintPixels pt,void *param);
/**
 * @brief 应该用等宽字库来构造，总共有94个连续字符　[!(33/0x21) ... ~(126/0x7e)]
 */
int ce_texture_font(CE_APP *app,TEXTURE_FONT *tFont, TTF_Font *font,SDL_Color color);
/**
 * @brief 在指定座标显示文字
 */
int ce_show_text(CE_APP *app,TEXTURE_FONT *tFont,const char *msg,int x,int y);

////////////////////////////////////////////////////////////////////////////////////////
/// command
////////////////////////////////////////////////////////////////////////////////////////


typedef int(*Command)(void *);

int cmds_close(THREAD_CMD *);
int cmds_init(THREAD_CMD *);
int cmds_put(THREAD_CMD *,Command cmd,void *);

////////////////////////////////////////////////////////////////////////////////////////
/// error macro
////////////////////////////////////////////////////////////////////////////////////////

#define ERROR(geterr)SDL_LogError(0,"%s(%d)%s",__FILE__,__LINE__,geterr());

#define HANDLER_NOT_ZERO_SDLERR_RET_NEG(act) if(act){SDL_LogError(0,"%s(%d)%s",__FILE__,__LINE__,SDL_GetError());return -1;}
#define HANDLER_NOT_ZERO_RET_NEG(act) if(act){SDL_LogError(0,"%s(%d):ERROR_WRAP",__FILE__,__LINE__);return -1;}