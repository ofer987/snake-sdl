#include <stdbool.h>
#include <stdlib.h>

#include "./constants.h"
#include "./coordinates.h"
#include "./snake.h"
#include "./types.h"

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

bool
move_left(Coordinates* coordinates) {
  if (coordinates->x < 1) {
    return false;
  }

  size_t new_x = coordinates->x - 1;
  size_t new_y = coordinates->y;

  move_snake(coordinates, new_x, new_y);
  return true;
}

bool
move_up(Coordinates* coordinates) {
  if (coordinates->y < 1) {
    return false;
  }

  size_t new_x = coordinates->x;
  size_t new_y = coordinates->y - 1;

  move_snake(coordinates, new_x, new_y);
  return true;
}

bool
move_right(Coordinates* coordinates) {
  // Otherwise, the right side of the box is painted
  // TODO(ofer987): fix renderer to not paint to the right edge of the screen
  if (coordinates->x >= HORIZONTAL_TILES_COUNT - 1) {
    return false;
  }

  size_t new_x = coordinates->x + 1;
  size_t new_y = coordinates->y;

  move_snake(coordinates, new_x, new_y);
  return true;
}

bool
move_down(Coordinates* coordinates) {
  // Otherwise, the bottom side of the box is painted
  // TODO(ofer987): fix renderer to not paint to the bottom edge of the screen
  if (coordinates->y >= VERTICAL_TILES_COUNT - 1) {
    return false;
  }

  size_t new_x = coordinates->x;
  size_t new_y = coordinates->y + 1;

  move_snake(coordinates, new_x, new_y);
  return true;
}

bool snake_inited = false;

// TODO(ofer987): Use NullObject pattern
static Snake SNAKE;

struct _Snake {
  struct coordinates_struct* head;

  size_t length;
};

Snake*
init_snake(size_t x, size_t y) {
  if (snake_inited) {
    return NULL;
  }

  SNAKE.length = 1;
  SNAKE.head = init_coordinates_x_y(x, y, USED_BY_SNAKE_HEAD);

  return &SNAKE;
}

Coordinates*
get_head(Snake* snake) {
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

bool
rerender_snake(Snake* snake, enum MOVEMENTS current_movement) {
  switch (current_movement) {
    case LEFT: return move_left(snake->head);
    case UP: return move_up(snake->head);
    case RIGHT: return move_right(snake->head);
    case DOWN: return move_down(snake->head);
    default: break;
  }

  return true;
}

bool
has_snaked_collided(Snake* snake) {
  if (has_collided_with_tail(snake)) {
    return true;
  }

  if (has_collided_with_border(snake)) {
    return false;
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

Coordinates*
get_snake_head(Snake* snake) {
  return snake->head;
}

/*  */
/* bool validate_left_boundary(size_t x) { */
/*   return x > WIDTH; */
/* } */
