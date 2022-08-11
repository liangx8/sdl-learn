/*抽象结构*/

struct SDL_APP;

struct SDL_APP *sdl_create(void);
void sdl_free(struct SDL_APP *ptr);
void sdl_run(struct SDL_APP *);
