#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "font_func.h"

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const char *FONT_PATH = "/usr/share/fonts/noto-cjk/NotoSerifCJK-Regular.ttc";
static const int FONT_SIZE = 48;
static const char *WELCOME_TEXT = "SDL2 例程";

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_Init Error: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "SDL2 例程",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Color white = {255, 255, 255, 255};
    SDL_Texture *textTexture = create_texture_by_string(renderer, FONT_PATH, FONT_SIZE, WELCOME_TEXT, white);
    if (!textTexture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "create_texture_by_string failed");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Rect textRect;
    if (SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_QueryTexture Error: %s", SDL_GetError());
        SDL_DestroyTexture(textTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    textRect.x = (WINDOW_WIDTH - textRect.w) / 2;
    textRect.y = (WINDOW_HEIGHT - textRect.h) / 2;

    NUMBER_TEMPLATE *counterTemplate = number_template(renderer, &white, 32);
    if (!counterTemplate) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "number_template failed");
        SDL_DestroyTexture(textTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Uint32 lastTick = SDL_GetTicks();
    int counter = 0;
    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        Uint32 now = SDL_GetTicks();
        if (now - lastTick >= 1000) {
            lastTick = now;
            counter += 1;
        }

        SDL_SetRenderDrawColor(renderer, 0, 120, 215, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        if (render_number(renderer, counterTemplate, counter, 20, 20) != 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "render_number failed");
            running = 0;
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    destroy_number_template(counterTemplate);
    SDL_DestroyTexture(textTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}