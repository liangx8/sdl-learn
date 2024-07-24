#pragma once


typedef struct _GAME GAME;
/**
 * @brief 新建游戏
 */
GAME* game_new(int rows,int cols);
/**
 * @brief 清除游戏数据
 */
void game_clear(GAME*);
#if 0
/**
 * @brief 旋转GAME::block
 */
int game_rotate(GAME *game);
/**
 * @brief 下移GAME::block
 */
int game_down(GAME *game);
/**
 * @brief 左移GAME::block
 */
int game_left(GAME *game);
/**
 * @brief 右移GAME::block
 */
int game_right(GAME *game);
#endif