#ifndef TEXTURE_FUNC_H
#define TEXTURE_FUNC_H

#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif
struct GRID_PARAM{
    // 底颜色
    SDL_Color bgcolor;
    int gridsize;
};
/**
 * @brief 画带格的背景图的回调函数
 * @param pixels 参考texture的修改回调
 * @param h
 * @param pitch
 * @param param 必须是struct GRID_PARAM*
 */
typedef int (*texture_paint_callback)(char *pixels, int h, int pitch, void *param);

/**
 * @brief Create a streaming RGBA texture and paint pixel data via callback.
 *
 * The created texture uses `SDL_PIXELFORMAT_RGBA8888` and is locked while the
 * `pt` callback writes pixel data. The callback receives a pointer to the
 * pixel buffer, the texture height `h`, the `pitch` (bytes per row), and the
 * user-supplied `param`.
 *
 * @param ren SDL renderer used to create the texture.
 * @param w Width of the texture in pixels.
 * @param h Height of the texture in pixels.
 * @param pt Callback that fills the pixel buffer. Should return 0 on success,
 *           non-zero to indicate failure (caller will destroy the texture).
 * @param param User pointer passed through to the callback.
 * @return A newly created `SDL_Texture*` on success, or `NULL` on error.
 */
SDL_Texture *create_texture_paint_pixels(SDL_Renderer *ren, int w, int h,
                                        texture_paint_callback pt, void *param);

/**
 * @brief Example callback that paints a solid color rectangle onto the texture.
 * @param pixels Pointer to the pixel buffer of the texture.
 * @param h Height of the texture in pixels.
 * @param pitch Number of bytes in a single row of the texture (width * 4 for RGBA8888).
 * @param param Pointer to an `SDL_Color` struct that defines the rectangle color.
 * @return 0 on success, non-zero on failure.
 */
int rect_texture_callback(char *pixels,int h,int pitch,void *param);                                        
/**
 * @brief Example callback that paints a grid pattern onto the texture.
 * @param pixels Pointer to the pixel buffer of the texture.
 * @param h Height of the texture in pixels.
 * @param pitch Number of bytes in a single row of the texture (width * 4 for RGBA8888).
 * @param param Pointer to a `GRID_PARAM` struct that defines the grid size and background color.
 * @return 0 on success, non-zero on failure.
 */
int grid_texture_callback(char *pixels, int h, int pitch, void *param);

/**
 * @brief Create a texture from UTF-8 text using the supplied font.
 *
 * The returned texture contains rendered text and must be destroyed by the
 * caller with SDL_DestroyTexture(). On error, this function pushes failure
 * details via app_err_push and returns NULL.
 *
 * @param renderer The SDL_Renderer used to create the texture.
 * @param font_path Path to a TrueType font file.
 * @param size Font point size to use for rendering.
 * @param text UTF-8 encoded null-terminated string to render.
 * @param color Text color.
 * @return A new SDL_Texture*, or NULL on error.
 */
SDL_Texture *create_texture_by_string(SDL_Renderer *renderer, const char *font_path, int size, const char *text, SDL_Color color);



/**
 * A template representation for digit rendering using a 0-9 texture strip.
 *
 * The template stores the texture containing digits 0-9 in a single row,
 * along with the fixed width and height of each digit cell.
 */
typedef struct NUMBER_TEMPLATE {
    SDL_Texture *texture;
    int digit_width;
    int digit_height;
} NUMBER_TEMPLATE;
/**
 * Create a number template from the digits 0-9 using a monospace font.
 *
 * @param renderer The SDL_Renderer used to create the texture.
 * @param color    Pointer to the SDL_Color to render the digits in.
 * @param size     Font size to use when rendering.
 * @return A newly allocated NUMBER_TEMPLATE on success, or NULL on error.
 */
NUMBER_TEMPLATE *number_template(SDL_Renderer *renderer,const char *font_path, const SDL_Color *color, int size);

/**
 * Destroy a previously created number template.
 *
 * @param tmpl The NUMBER_TEMPLATE to destroy.
 */
void destroy_number_template(NUMBER_TEMPLATE *tmpl);

/**
 * Render an integer at the specified screen position using a digit template.
 *
 * @param renderer The SDL_Renderer used to draw the number.
 * @param tmpl     The NUMBER_TEMPLATE returned by number_template().
 * @param number   The number to render. Only non-negative values are supported.
 * @param x        The x coordinate of the left edge of the rendered number.
 * @param y        The y coordinate of the top edge of the rendered number.
 * @return 0 on success, or -1 on error.
 */
int render_number(SDL_Renderer *renderer, const NUMBER_TEMPLATE *tmpl, int number, int x, int y);

/**
 * @brief Render a single symbol at the specified screen position using a digit template. Supported symbols are '0'-'9', '+', '-', and '='.
 * @param renderer The SDL_Renderer used to draw the symbol.
 * @param tmpl The NUMBER_TEMPLATE returned by number_template().
 * @param symbol The character to render. Supported symbols are '0'-'9', '+', '-', and '='.
 * @param x The x coordinate of the left edge of the rendered symbol.
 * @param y The y coordinate of the top edge of the rendered symbol.
 * @return 0 on success, or -1 on error.
 */
int render_symbol(SDL_Renderer *renderer, const NUMBER_TEMPLATE *tmpl, char symbol, int x, int y);
#ifdef __cplusplus
}
#endif

#endif /* TEXTURE_FUNC_H */
