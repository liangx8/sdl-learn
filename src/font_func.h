#ifndef FONT_FUNC_H
#define FONT_FUNC_H
#include <SDL2/SDL.h>

/**
 * @file font_func.h
 * @brief Helper functions to create SDL surfaces and textures from UTF-8 text using SDL_ttf.
 */


/**
 * Create an SDL_Texture from UTF-8 text using the given renderer and color.
 *
 * @param renderer  The SDL_Renderer used to create the texture.
 * @param font_path Path to a TrueType font file (TTF/OTF).
 * @param size      Font point size to use when rendering.
 * @param text      UTF-8 encoded NUL-terminated string to render.
 * @param color     Color to render the text in.
 * @return A pointer to a newly created SDL_Texture on success. The caller is
 *         responsible for calling SDL_DestroyTexture on the returned texture.
 *         Returns NULL on error (and logs the error via SDL_LogError).
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
NUMBER_TEMPLATE *number_template(SDL_Renderer *renderer, const SDL_Color *color, int size);

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

#endif
