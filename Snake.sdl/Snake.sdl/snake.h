#ifndef SNAKE_SDL_SNAKE_SDL_SNAKE_H_
#define SNAKE_SDL_SNAKE_SDL_SNAKE_H_

#include <stdbool.h>
#include <stdlib.h>

#include "./coordinates.h"
#include "./types.h"

struct _Snake;
typedef struct _Snake Snake;

Snake* init_snake(size_t x, size_t y);

Coordinates* get_head(Snake*);

void rerender_snake(Snake* snake, enum MOVEMENTS current_movement);

bool has_just_eaten_food(Snake*, Coordinates*);
Coordinates* get_snake_head(Snake* snake);
size_t get_snake_length(Snake* snake);
bool has_snaked_collided(Snake* snake, Coordinates** screen);
void destroy_snake(Snake* snake);

#endif // SNAKE_SDL_SNAKE_SDL_SNAKE_H_
