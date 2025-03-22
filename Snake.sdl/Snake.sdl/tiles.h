#ifndef SNAKE_SDL_SNAKE_SDL_TILES_H_
#define SNAKE_SDL_SNAKE_SDL_TILES_H_

#include <stdlib.h>

#include "./coordinates.h"

struct _Tiles;
typedef struct _Tiles Tiles;

Tiles* init_tiles(size_t, size_t);
void destroy_tiles(void);

void rerender_tiles(Coordinates*, Coordinates*);

enum TILE_TYPES get_tile_type(size_t index);

#endif // SNAKE_SDL_SNAKE_SDL_TILES_H_
