#include <stdbool.h>
#include <stdlib.h>

#include "./coordinates.h"
#include "./game.h"
#include "./snake.h"
#include "./types.h"

struct _Game {
  enum GAME_MODES mode;

  Coordinates** available_tiles;
  size_t available_tiles_count;

  Coordinates** screen_indexer;
  Coordinates* screen;

  Coordinates* food;
  Snake* snake;
  enum MOVEMENTS current_movement;

  Coordinates* top_left_corner;
  Coordinates* top_right_corner;
  Coordinates* bottom_left_corner;
  Coordinates* bottom_right_corner;
};

Snake*
_init_snake(size_t width, size_t height) {
  size_t middle_x = (height % 2) ? ((height - 1) / 2) : (height / 2);
  size_t middle_y = (width % 2) ? ((width - 1) / 2) : (width / 2);

  return init_snake(middle_x, middle_y);
}

void
_init_screen(Game* game, size_t total) {
  game->screen_indexer = malloc(total * sizeof(Coordinates*));
  game->available_tiles = malloc(total * sizeof(Coordinates*));

  Coordinates* current;
  Coordinates** prev = &game->screen;

  for (size_t index = 0; index < total; index += 1) {
    current = init_coordinates(index);

    *prev = current;
    prev = &current->next;

    game->screen_indexer[index] = current;
  }
}

Game*
init_game(size_t width, size_t height) {
  if (width <= 0) {
    exit(EXIT_FAILURE);
  }

  if (height <= 0) {
    exit(EXIT_FAILURE);
  }

  Game* game = malloc(sizeof(Game));
  Snake* snake = _init_snake(width, height);
  _init_screen(game, (width) * (height));

  game->mode = START;
  game->snake = snake;
  game->food = init_food();

  game->top_left_corner = init_coordinates_x_y(0, 0, USED_BY_TOP_LEFT_BORDER);
  game->top_right_corner = init_coordinates_x_y(width, 0, USED_BY_TOP_RIGHT_BORDER);

  game->bottom_left_corner = init_coordinates_x_y(0, height, USED_BY_BOTTOM_LEFT_BORDER);
  game->bottom_right_corner = init_coordinates_x_y(width, height, USED_BY_BOTTOM_RIGHT_BORDER);

  return game;
}

Snake*
get_snake_location(Game* game) {
  return game->snake;
}

Coordinates*
get_food_location(Game* game) {
  return game->food;
}

enum MOVEMENTS
get_current_movement(Game* game) {
  return game->current_movement;
}

void
set_current_movement(Game* game, enum MOVEMENTS new_movement) {
  game->current_movement = new_movement;
}

void
reinit_screen(Coordinates* screen) {
  Coordinates* current = screen;
  while (current != NULL) {
    current->type = AVAILABLE;

    current = current->next;
  }
}

void
count_available_tiles(Game* game) {
  Coordinates* tile = game->screen;

  size_t index = 0;
  while (tile != NULL) {
    if (tile->type == AVAILABLE) {
      game->available_tiles[index] = tile;
    }

    index += 1;

    tile = tile->next;
  }

  game->available_tiles_count = index;
}

void
rerender_screen(Game* game) {
  reinit_screen(game->screen);

  Snake* snake = game->snake;
  Coordinates* food = game->food;
  size_t index;
  size_t available_tile_count = 0;

  // Snake
  Coordinates* coordinates = get_head(snake);
  enum TILE_TYPES type = USED_BY_SNAKE_HEAD;
  while (coordinates != NULL) {
    index = coordinates_to_index(coordinates->x, coordinates->y);
    if (index >= 770) {
      exit(EXIT_FAILURE);
    }

    game->screen_indexer[index]->type = type;

    type = USED_BY_SNAKE_TAIL;
    coordinates = coordinates->next;
  }

  // Food
  index = coordinates_to_index(food->x, food->y);
  game->screen_indexer[index]->type = USED_BY_FOOD;

  count_available_tiles(game);
}

Coordinates*
get_screen(Game* game) {
  return game->screen;
}

bool
change_food_location(Game* game) {
  if (game->available_tiles_count == 0) {
    return false;
  }

  size_t random_number = random() / (RAND_MAX / game->available_tiles_count);

  Coordinates* available_tile = game->available_tiles[random_number];

  game->food->x = available_tile->x;
  game->food->y = available_tile->y;

  return true;
}

Coordinates*
get_top_left_corner(Game* game) {
  return game->top_left_corner;
}

Coordinates*
get_bottom_left_corner(Game* game) {
  return game->bottom_left_corner;
}

Coordinates*
get_top_right_corner(Game* game) {
  return game->top_right_corner;
}

Coordinates*
get_bottom_right_corner(Game* game) {
  return game->bottom_right_corner;
}

enum GAME_MODES
get_game_mode(Game* game) {
  return game->mode;
}

void
set_game_mode(Game* game, enum GAME_MODES mode) {
  game->mode = mode;
}
