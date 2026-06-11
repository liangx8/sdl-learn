#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "font_func.h"

/**
 * @file font_func.c
 * @brief Implementations of helpers that render UTF-8 text via SDL_ttf.
 */

const char *mono_font_path = "/usr/share/fonts/noto/NotoSansMono-Medium.ttf";


/**
 * Create an SDL_Texture from UTF-8 text using the given renderer and color.
 *
 * This function opens the font at @p font_path with the requested @p size,
 * renders @p text to a temporary surface and converts it to a texture via the
 * supplied @p renderer.
 *
 * @param renderer  The SDL_Renderer used to create the texture.
 * @param font_path Path to a TrueType font file (TTF/OTF).
 * @param size      Font point size to use when rendering.
 * @param text      UTF-8 encoded NUL-terminated string to render.
 * @param color     Color to render the text in.
 * @return A pointer to a newly created SDL_Texture on success (caller must
 *         destroy with SDL_DestroyTexture). Returns NULL on error and logs
 *         the failure via SDL_LogError.
 */
SDL_Texture *create_texture_by_string(SDL_Renderer *renderer, const char *font_path, int size, const char *text, SDL_Color color)
{
    if (!renderer || !font_path || !text || size <= 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "create_texture_by_string: invalid arguments");
        return NULL;
    }

    TTF_Font *font = TTF_OpenFont(font_path, size);
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont Error: %s", TTF_GetError());
        return NULL;
    }

    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_RenderUTF8_Blended Error: %s", TTF_GetError());
        TTF_CloseFont(font);
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
    }

    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    return texture;
}
NUMBER_TEMPLATE *number_template(SDL_Renderer *renderer,const SDL_Color *color,int size){
    if (!renderer || !color || size <= 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "number_template: invalid arguments");
        return NULL;
    }

    SDL_Texture *texture = create_texture_by_string(renderer, mono_font_path, size, "0123456789", *color);
    if (!texture) {
        return NULL;
    }

    int tex_w = 0;
    int tex_h = 0;
    if (SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_QueryTexture Error: %s", SDL_GetError());
        SDL_DestroyTexture(texture);
        return NULL;
    }

    if (tex_w <= 0 || tex_h <= 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "number_template: invalid texture dimensions");
        SDL_DestroyTexture(texture);
        return NULL;
    }

    NUMBER_TEMPLATE *tmpl = (NUMBER_TEMPLATE *)SDL_malloc(sizeof(NUMBER_TEMPLATE));
    if (!tmpl) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "number_template: allocation failed");
        SDL_DestroyTexture(texture);
        return NULL;
    }

    tmpl->texture = texture;
    tmpl->digit_width = tex_w / 10;
    tmpl->digit_height = tex_h;
    return tmpl;
}

void destroy_number_template(NUMBER_TEMPLATE *tmpl){
    if (!tmpl) {
        return;
    }
    if (tmpl->texture) {
        SDL_DestroyTexture(tmpl->texture);
    }
    SDL_free(tmpl);
}

/**
 * Render a non-negative integer to the current renderer using the digit template.
 *
 * @param renderer The SDL_Renderer to draw onto.
 * @param tmpl     The NUMBER_TEMPLATE returned by number_template().
 * @param number   The integer to render; must be >= 0.
 * @param x        The x coordinate of the left edge of the rendered number.
 * @param y        The y coordinate of the top edge of the rendered number.
 * @return 0 on success, -1 on error.
 */
int render_number(SDL_Renderer *renderer,const NUMBER_TEMPLATE *tmpl,int number,int x,int y){
    if (!renderer || !tmpl || number < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "render_number: invalid arguments");
        return -1;
    }

    if (!tmpl->texture || tmpl->digit_width <= 0 || tmpl->digit_height <= 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "render_number: invalid template");
        return -1;
    }

    char digits[32];
    int length = SDL_snprintf(digits, sizeof(digits), "%d", number);
    if (length <= 0 || length >= (int)sizeof(digits)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "render_number: invalid number conversion");
        return -1;
    }

    for (int i = 0; i < length; ++i) {
        char ch = digits[i];
        if (ch < '0' || ch > '9') {
            continue;
        }

        int index = ch - '0';
        SDL_Rect src = { index * tmpl->digit_width, 0, tmpl->digit_width, tmpl->digit_height };
        SDL_Rect dst = { x + i * tmpl->digit_width, y, tmpl->digit_width, tmpl->digit_height };

        if (SDL_RenderCopy(renderer, tmpl->texture, &src, &dst) != 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_RenderCopy Error: %s", SDL_GetError());
            return -1;
        }
    }

    return 0;
}
