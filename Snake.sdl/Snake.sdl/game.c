#include <stdbool.h>
#include <stdlib.h>

#include "./coordinates.h"
#include "./game.h"
#include "./snake.h"
#include "./types.h"

struct _Game {
  enum GAME_MODES mode;
  enum CHEAT_CODES enabled_cheat_codes;

  Coordinates** screen;

  size_t available_tiles_count;
  Coordinates** available_tiles;

  Coordinates* food;
  Snake* snake;
  enum MOVEMENTS current_movement;

  size_t width, height;

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

Coordinates**
_init_screen(size_t width, size_t height) {
  Coordinates** result = malloc(width * height * sizeof(Coordinates*));

  size_t index = 0;
  for (size_t y = 0; y < height; y += 1) {
    for (size_t x = 0; x < width; x += 1) {
      index = x_y_to_index(x, y);

      Coordinates* coordinates = init_coordinates_x_y(x, y, AVAILABLE);
      result[index] = coordinates;
    }
  }

  return result;
}

void
render_borders(Coordinates** screen, Coordinates* top_left_corner, Coordinates* top_right_corner,
               Coordinates* bottom_left_corner, Coordinates* bottom_right_corner) {
  size_t index = x_y_to_index(top_left_corner->x, top_left_corner->y);
  screen[index]->type |= USED_BY_TOP_LEFT_BORDER;

  index = x_y_to_index(top_right_corner->x, top_right_corner->y);
  screen[index]->type |= USED_BY_TOP_RIGHT_BORDER;

  index = x_y_to_index(bottom_left_corner->x, bottom_left_corner->y);
  screen[index]->type |= USED_BY_BOTTOM_LEFT_BORDER;

  index = x_y_to_index(bottom_right_corner->x, bottom_right_corner->y);
  screen[index]->type |= USED_BY_BOTTOM_RIGHT_BORDER;

  // Render TOP border
  size_t y = 0;
  for (size_t x = 1; x < top_right_corner->x; x += 1) {
    index = x_y_to_index(x, y);
    screen[index]->type |= USED_BY_TOP_BORDER;
  }

  // Render LEFT border
  size_t x = 0;
  for (size_t y = 1; y < bottom_left_corner->y; y += 1) {
    index = x_y_to_index(x, y);
    screen[index]->type |= USED_BY_LEFT_BORDER;
  }

  // Render BOTTOM border
  y = bottom_right_corner->y;
  for (size_t x = 1; x < bottom_right_corner->x; x += 1) {
    index = x_y_to_index(x, y);
    screen[index]->type |= USED_BY_BOTTOM_BORDER;
  }

  // Render RIGHT border
  x = top_right_corner->x;
  for (size_t y = 1; y < bottom_right_corner->y; y += 1) {
    index = x_y_to_index(x, y);
    screen[index]->type |= USED_BY_RIGHT_BORDER;
  }
}

void
destroy_screen(Coordinates** screen, size_t screen_size) {
  for (size_t index = 0; index < screen_size; index += 1) {
    Coordinates* coordinates = screen[index];
    destroy_coordinates(coordinates);
  }

  free(screen);
  screen = NULL;
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
set_available_tiles(Game* game) {
  size_t index = 0;

  Coordinates* coordinate = get_screen_coordinate(game, index);
  size_t available_tiles_count = 0;
  while (coordinate != NULL) {
    if (coordinate->type == AVAILABLE) {
      game->available_tiles[available_tiles_count] = coordinate;

      available_tiles_count += 1;
    }

    index += 1;
    coordinate = get_screen_coordinate(game, index);
  }

  game->available_tiles_count = available_tiles_count;
}

void
render_snake(Coordinates** screen, Coordinates* snake_head) {
  Coordinates* coordinates = snake_head;
  enum TILE_TYPES type = USED_BY_SNAKE_HEAD;
  while (coordinates != NULL) {
    size_t index = x_y_to_index(coordinates->x, coordinates->y);
    screen[index]->type |= type;

    type = USED_BY_SNAKE_TAIL;
    coordinates = coordinates->next;
  }
}

void
render_food(Coordinates** screen, Coordinates* food) {
  size_t index = coordinates_to_index(food->x, food->y);
  screen[index]->type |= USED_BY_FOOD;
}

void
render_screen(Game* game) {
  game->screen = _init_screen(game->width, game->height);

  render_borders(game->screen, game->top_left_corner, game->top_right_corner, game->bottom_left_corner,
                 game->bottom_right_corner);

  Coordinates* snake_head = get_snake_head(game->snake);
  render_snake(game->screen, snake_head);
  render_food(game->screen, game->food);

  set_available_tiles(game);
}

Coordinates*
get_screen_coordinate(Game* game, size_t index) {
  if (index >= game->width * game->height) {
    return NULL;
  }

  return game->screen[index];
}

Coordinates**
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

Game*
init_game(size_t width, size_t height) {
  if (width <= 0) {
    exit(EXIT_FAILURE);
  }

  if (height <= 0) {
    exit(EXIT_FAILURE);
  }

  Game* game = malloc(sizeof(Game));
  // TODO(ofer987: maybe send width and height as +2 for the borders
  game->width = width + 2;
  game->height = height + 2;

  game->mode = START;
  Snake* snake = _init_snake(game->width, game->height);
  game->snake = snake;
  game->food = init_food();

  game->top_left_corner = init_coordinates_x_y(0, 0, USED_BY_TOP_LEFT_BORDER);
  game->top_right_corner = init_coordinates_x_y(game->width - 1, 0, USED_BY_TOP_RIGHT_BORDER);

  game->bottom_left_corner = init_coordinates_x_y(0, game->height - 1, USED_BY_BOTTOM_LEFT_BORDER);
  game->bottom_right_corner = init_coordinates_x_y(game->width - 1, game->height - 1, USED_BY_BOTTOM_RIGHT_BORDER);

  game->available_tiles = malloc(game->width * game->height * sizeof(Coordinates*));
  render_screen(game);

  return game;
}

void
rerender_screen(Game* game) {
  destroy_screen(game->screen, game->width * game->height);
  game->screen = NULL;

  render_screen(game);
}

void
destroy_game(Game* game) {
  destroy_screen(game->screen, game->width * game->height);
  game->screen = NULL;

  free(game->available_tiles);
  game->available_tiles = NULL;

  free(game->top_left_corner);
  game->top_left_corner = NULL;

  free(game->top_right_corner);
  game->top_right_corner = NULL;

  free(game->bottom_right_corner);
  game->bottom_right_corner = NULL;

  free(game->bottom_left_corner);
  game->bottom_left_corner = NULL;

  game->current_movement = NOTHING;

  destroy_snake(game->snake);
  game->snake = NULL;

  free(game);
  game = NULL;
}

void
set_cheat_codes(Game* game, enum CHEAT_CODES cheat_codes) {
  game->enabled_cheat_codes ^= cheat_codes;
}

bool
no_death(Game* game) {
  return (game->enabled_cheat_codes & NO_DEATH) > 0;
}
