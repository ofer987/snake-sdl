#ifndef SNAKE_SDL_SNAKE_SDL_GAME_H_
#define SNAKE_SDL_SNAKE_SDL_GAME_H_

#include <stdbool.h>
#include <stdlib.h>

#include "./coordinates.h"
#include "./snake.h"
#include "./types.h"

enum GAME_MODES { START, CONTINUE, PAUSE, WIN, QUIT };

struct _Game;
typedef struct _Game Game;

Game* init_game(size_t width, size_t height);

Snake* get_snake_location(Game* game);
Coordinates* get_food_location(Game* game);

enum MOVEMENTS get_current_movement(Game*);
void set_current_movement(Game*, enum MOVEMENTS);
void rerender_screen(Game*);

Coordinates** get_screen(Game*);

bool change_food_location(Game*);

Coordinates* get_top_left_corner(Game* game);
Coordinates* get_bottom_left_corner(Game* game);
Coordinates* get_top_right_corner(Game* game);
Coordinates* get_bottom_right_corner(Game* game);

Coordinates* get_screen_coordinate(Game* game, size_t index);
enum GAME_MODES get_game_mode(Game* game);
void set_game_mode(Game* game, enum GAME_MODES mode);
void set_cheat_codes(Game* game, enum CHEAT_CODES cheat_codes);
bool no_death(Game* game);

void destroy_game(Game* game);
#endif // SNAKE_SDL_SNAKE_SDL_GAME_H_
