#include <stdbool.h>
#include <stdlib.h>

#include "game.h"

struct coordinates_struct*
create_head(size_t width, size_t height) {
  struct coordinates_struct* head = malloc(sizeof(struct coordinates_struct));

  size_t middle_x = (height % 2) ? ((height - 1) / 2) : (height / 2);
  size_t middle_y = (width % 2) ? ((width - 1) / 2) : (width / 2);

  head->next = NULL;
  head->x = middle_x;
  head->y = middle_y;
  head->type = USED_BY_SNAKE_HEAD;

  return head;
}

struct game_struct*
init_game(size_t width, size_t height) {
  if (width <= 0) {
    exit(EXIT_FAILURE);
  }

  if (height <= 0) {
    exit(EXIT_FAILURE);
  }

  struct game_struct* game = malloc(sizeof(struct game_struct));
  struct snake_struct* snake = malloc(sizeof(struct snake_struct));

  snake->current_movement = RIGHT;
  snake->head = create_head(width, height);
  snake->length = 1;

  game->mode = START;
  game->snake = snake;

  return game;
}
