#include <stdlib.h>
#include <time.h>

#include "./constants.h"
#include "./coordinates.h"
#include "./types.h"

bool is_food_generated = false;
static Coordinates FOOD;

size_t
x_coordinate(struct coordinates_struct coordinates) {
  return coordinates.x;
}

size_t
y_coordinate(struct coordinates_struct coordinates) {
  return coordinates.y;
}

Coordinates*
init_coordinates(size_t index) {
  Coordinates* result = malloc(sizeof(Coordinates));

  size_t y = index / (HORIZONTAL_TILES_COUNT + 2);
  size_t x = index % (HORIZONTAL_TILES_COUNT + 2);

  result->x = x;
  result->y = y;

  return result;
}

Coordinates*
init_coordinates_x_y(size_t x, size_t y, enum TILE_TYPES type) {
  Coordinates* result = malloc(sizeof(Coordinates));

  result->x = x;
  result->y = y;
  result->type = type;

  return result;
}

void
destroy_coordinates(Coordinates* coordinates) {
  free(coordinates);
}

size_t
x_y_to_index(size_t x, size_t y) {
  return y * (HORIZONTAL_TILES_COUNT + 2) + x;
}

size_t
coordinates_to_index(size_t x, size_t y) {
  return (HORIZONTAL_TILES_COUNT + 2) * y + x;
}

struct coordinates_struct*
init_food(void) {
  if (is_food_generated) {
    return NULL;
  }

  FOOD.type = USED_BY_FOOD;
  FOOD.x = 10;
  FOOD.y = 10;

  return &FOOD;
}
