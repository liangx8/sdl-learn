/*抽象结构*/
#ifndef ABS_STRUCT_H_KKDIIDKKS324J3
#define ABS_STRUCT_H_KKDIIDKKS324J3
struct SDL_APP;

struct SDL_APP *sdl_create(void);
void sdl_cleanup(struct SDL_APP *ptr);
void sdl_run(struct SDL_APP *);
#endif