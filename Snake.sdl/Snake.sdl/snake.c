#include <stdbool.h>
#include <stdlib.h>

#include "./coordinates.h"
#include "./snake.h"
#include "./types.h"

bool
has_collided(enum TILE_TYPES tile_type) {
  switch (tile_type) {
    case USED_BY_SNAKE_TAIL:
      /* FALLTHROUGH */
    case USED_BY_LEFT_BORDER:
      /* FALLTHROUGH */
    case USED_BY_RIGHT_BORDER:
      /* FALLTHROUGH */
    case USED_BY_TOP_BORDER:
      /* FALLTHROUGH */
    case USED_BY_BOTTOM_BORDER:
      /* FALLTHROUGH */
    case USED_BY_TOP_LEFT_BORDER:
      /* FALLTHROUGH */
    case USED_BY_TOP_RIGHT_BORDER:
      /* FALLTHROUGH */
    case USED_BY_BOTTOM_LEFT_BORDER:
      /* FALLTHROUGH */
    case USED_BY_BOTTOM_RIGHT_BORDER: return true;
    default: return false;
  };
}

void
move_snake(Coordinates* coordinates, size_t new_x, size_t new_y) {
  while (coordinates != NULL) {
    size_t temp_x = coordinates->x;
    size_t temp_y = coordinates->y;

    coordinates->x = new_x;
    coordinates->y = new_y;

    new_x = temp_x;
    new_y = temp_y;

    coordinates = coordinates->next;
  }
}

void
move_left(Coordinates* coordinates) {
  size_t new_x = coordinates->x - 1;
  size_t new_y = coordinates->y;

  move_snake(coordinates, new_x, new_y);
}

void
move_up(Coordinates* coordinates) {
  size_t new_x = coordinates->x;
  size_t new_y = coordinates->y - 1;

  move_snake(coordinates, new_x, new_y);
}

void
move_right(Coordinates* coordinates) {
  size_t new_x = coordinates->x + 1;
  size_t new_y = coordinates->y;

  move_snake(coordinates, new_x, new_y);
}

void
move_down(Coordinates* coordinates) {
  size_t new_x = coordinates->x;
  size_t new_y = coordinates->y + 1;

  move_snake(coordinates, new_x, new_y);
}

bool snake_inited = false;

struct _Snake {
  struct coordinates_struct* head;

  size_t length;
};

static struct _Snake SNAKE = { .head = NULL, .length = 0 };

Snake*
init_snake(size_t x, size_t y) {
  if (snake_inited) {
    return &SNAKE;
  }

  SNAKE.length = 1;
  SNAKE.head = init_coordinates_x_y(x, y, USED_BY_SNAKE_HEAD);

  return &SNAKE;
}

Coordinates*
get_snake_head(Snake* snake) {
  return snake->head;
}

bool
has_collided_with_border(Snake* snake) {
  Coordinates* head = snake->head;

  if (head->x == 0) {
    return false;
  }
  Coordinates* tail = head->next;
  while (tail != NULL) {
    if (head->x == tail->x && head->y == tail->y) {
      return true;
    }

    tail = tail->next;
  }

  return false;
}

bool
has_collided_with_tail(Snake* snake) {
  Coordinates* head = snake->head;

  Coordinates* tail = head->next;
  while (tail != NULL) {
    if (head->x == tail->x && head->y == tail->y) {
      return true;
    }

    tail = tail->next;
  }

  return false;
}

void
rerender_snake(Snake* snake, enum MOVEMENTS current_movement) {
  switch (current_movement) {
    case LEFT: move_left(snake->head); break;
    case UP: move_up(snake->head); break;
    case RIGHT: move_right(snake->head); break;
    case DOWN: move_down(snake->head); break;
    default: break;
  }
}

bool
has_snaked_collided(Snake* snake, Coordinates** screen) {
  size_t index = x_y_to_index(snake->head->x, snake->head->y);
  // 0b100000000000 is the Snake head
  // 0b000000000100 is the Snake tail
  // And the borders are equal or greater than
  // 0b000000001000.
  // Therefore,
  // there is a collision if the number is at least 0b100000000100
  if (screen[index]->type >= 0b100000000100) {
    return true;
  }

  return false;
}

Coordinates*
init_new_snake_head(size_t x, size_t y) {
  Coordinates* result = init_coordinates_x_y(x, y, USED_BY_SNAKE_HEAD);

  return result;
}

size_t
get_snake_length(Snake* snake) {
  return snake->length;
}

bool
has_just_eaten_food(Snake* snake, Coordinates* food) {
  if (snake->head->x != food->x || snake->head->y != food->y) {
    return false;
  }

  Coordinates* new_snake_head = init_new_snake_head(food->x, food->y);

  Coordinates* new_tail = snake->head;

  snake->head = new_snake_head;
  snake->head->next = new_tail;

  snake->length += 1;

  return true;
}

void
destroy_snake(Snake* snake) {
  Coordinates* coordinates = snake->head;
  while (coordinates != NULL) {
    Coordinates* next_coordinates = coordinates->next;
    free(coordinates);

    coordinates = next_coordinates;
  }

  SNAKE.head = NULL;
  SNAKE.length = 0;

  snake_inited = false;
}
