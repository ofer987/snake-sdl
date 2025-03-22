#include <stdbool.h>
#include <stdlib.h>

#include "./coordinates.h"
#include "./types.h"

typedef struct _Tiles {
  size_t width;
  size_t height;

  enum TILE_TYPES* types;
} Tiles;

static Tiles RENDERED_TILES;
static bool HAVE_TILES_BEEN_RENDERED = false;

Tiles*
init_tiles(size_t width, size_t height) {
  if (HAVE_TILES_BEEN_RENDERED) {
    return &RENDERED_TILES;
  }

  RENDERED_TILES.width = width;
  RENDERED_TILES.height = height;

  RENDERED_TILES.types = malloc(width * height * sizeof(enum TILE_TYPES));

  return &RENDERED_TILES;
}

void
destroy_tiles(void) {
  free(RENDERED_TILES.types);

  HAVE_TILES_BEEN_RENDERED = false;
}

void
rerender_tiles(Coordinates* snake, Coordinates* food) {
  Coordinates* coordinates = snake;
  size_t index;

  // Snake
  enum TILE_TYPES type = USED_BY_SNAKE_HEAD;
  while (coordinates != NULL) {
    index = coordinates_to_index(coordinates->x, coordinates->y);
    RENDERED_TILES.types[index] = type;

    type = USED_BY_SNAKE_TAIL;
    coordinates = snake->next;
  }

  // Food
  index = coordinates_to_index(food->x, food->y);
  RENDERED_TILES.types[index] = USED_BY_FOOD;
}

enum TILE_TYPES
get_tile_type(size_t index) {
  return RENDERED_TILES.types[index];
}
