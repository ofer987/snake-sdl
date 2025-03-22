#ifndef SNAKE_SDL_SNAKE_SDL_GAME_H_
#define SNAKE_SDL_SNAKE_SDL_GAME_H_

#include <stdbool.h>
#include <stdlib.h>

enum TYPES {
  AVAILABLE = 1,
  USED_BY_SNAKE_TAIL,
  USED_BY_SNAKE_HEAD,
  USED_BY_FOOD,
  USED_BY_TOP_BORDER,
  USED_BY_TOP_RIGHT_BORDER,
  USED_BY_RIGHT_BORDER,
  USED_BY_BOTTOM_RIGHT_BORDER,
  USED_BY_BOTTOM_BORDER,
  USED_BY_BOTTOM_LEFT_BORDER,
  USED_BY_LEFT_BORDER,
  USED_BY_TOP_LEFT_BORDER,
  USED_BY_MESSAGE,
  TOTAL
};
enum MOVEMENTS { LEFT, UP, RIGHT, DOWN };
enum GAME_MODES { START, CONTINUE, PAUSE, WIN, QUIT };

struct coordinates_struct {
  enum TYPES type;
  size_t x;
  size_t y;

  struct coordinates_struct* next;
};

struct snake_struct {
  struct coordinates_struct* head;
  size_t length;

  enum MOVEMENTS current_movement;
};

struct game_struct {
  enum GAME_MODES mode;

  struct snake_struct* snake;
};

struct game_struct*
init_game(size_t width, size_t height);

#endif  // SNAKE_SDL_SNAKE_SDL_GAME_H_
