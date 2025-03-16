#ifndef SNAKE_SDL_SNAKE_SDL_COORDINATES_H_
#define SNAKE_SDL_SNAKE_SDL_COORDINATES_H_

#include <stdbool.h>
#include <stdlib.h>

#include "./constants.h"
#include "./types.h"

size_t x_coordinate(struct coordinates_struct coordinates);

size_t y_coordinate(struct coordinates_struct coordinates);

Coordinates* init_coordinates(size_t index);

Coordinates* init_coordinates_x_y(size_t x, size_t y, enum TILE_TYPES type);

void destroy_coordinates(Coordinates* coordinates);
size_t x_y_to_index(size_t x, size_t y);

size_t coordinates_to_index(size_t x, size_t y);

struct coordinates_struct* init_food(void);

#endif // SNAKE_SDL_SNAKE_SDL_COORDINATES_H_
