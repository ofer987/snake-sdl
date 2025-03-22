/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
/* #include "tiles.h" */
#include <stdlib.h>
#include "./types.h"

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "./constants.h"
#include "./game.h"
#include "./snake.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static float right_movement_span = 0;
static SDL_FRect frect = {.x = 50, .y = 100, .w = 10, .h = 10};

static Uint64 framerate_time = 0.0f;

/* enum MOVEMENTS { LEFT, UP, RIGHT, DOWN }; */
/* static enum MOVEMENTS movement; */
static Game* game;
/* static Tiles* tiles; */
/* static size_t tiles_count = HORIZONTAL_TILES_COUNT * VERTICAL_TILES_COUNT; */
static Coordinates coord;

/* static struct snake_struct* snake; */

/* This function runs once at startup. */
SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[]) {
  srandom(time(NULL));

  /* SDL_CursorVisible(); */
  /* SDL_ShowCursor(); */
  bool result = SDL_HideCursor();

  /* Create the window */
  if (!SDL_CreateWindowAndRenderer("Hello World", RESOLUTION_WIDTH, RESOLUTION_HEIGHT, SDL_WINDOW_FULLSCREEN, &window,
                                   &renderer)) {
    SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  /* tiles = init_tiles(HORIZONTAL_TILES_COUNT, VERTICAL_TILES_COUNT); */
  game = init_game(HORIZONTAL_TILES_COUNT, VERTICAL_TILES_COUNT);
  coord.type = USED_BY_FOOD;
  /* snake = game->snake; */
  /* frect.x = snake->head->x; */
  /* frect.y = snake->head->y; */

  return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }

  enum GAME_MODES game_mode = get_game_mode(game);
  enum MOVEMENTS current_movement = get_current_movement(game);

  if (event->type == SDL_EVENT_KEY_DOWN) {
    SDL_Keycode key = event->key.key;
    switch (key) {
      case SDLK_Q: return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
    }

    if (game_mode != QUIT) {
      switch (key) {
        case SDLK_LEFT:
        case SDLK_H:
          if (current_movement != RIGHT) {
            set_current_movement(game, LEFT);
          }

          break;
          /* set_current_movement(game, LEFT); break; */
        case SDLK_UP:
        case SDLK_K:
          if (current_movement != DOWN) {
            set_current_movement(game, UP);
          }

          break;
        case SDLK_RIGHT:
        case SDLK_L:
          if (current_movement != LEFT) {
            set_current_movement(game, RIGHT);
          }

          break;
        case SDLK_DOWN:
        case SDLK_J:
          if (current_movement != UP) {
            set_current_movement(game, DOWN);
          }

          break;
      }
    }
  }

  return SDL_APP_CONTINUE;
}

void
render_text(size_t score) {
  const float scale = 4.0f;

  SDL_SetRenderScale(renderer, scale, scale);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugTextFormat(renderer, 0, MAZE_HEIGHT - 20, "Score is: %zu", score);
}

void
render_failure() {
  const float scale = 4.0f;

  SDL_SetRenderScale(renderer, scale, scale);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugText(renderer, 0, MAZE_HEIGHT - 10, "Lost");
  SDL_RenderDebugText(renderer, 0, MAZE_HEIGHT, "Press Q to Quit");
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate) {
  Uint64 new_framerate_time = SDL_GetTicks();

  if (new_framerate_time < framerate_time + 100) {
    return SDL_APP_CONTINUE;
  }

  framerate_time = new_framerate_time;

  Snake* snake = get_snake_location(game);

  enum MOVEMENTS current_movement = get_current_movement(game);
  bool no_border_collision = rerender_snake(snake, current_movement);
  bool is_there_a_collision = has_snaked_collided(snake);

  /* render_failure(); */
  if (get_game_mode(game) == QUIT) {
    /* render_failure(); */
    set_current_movement(game, NOTHING);
  }

  if (!no_border_collision || is_there_a_collision) {
    // TOOD(ofer987): Display end game message
    /* render_failure(); */
    set_current_movement(game, NOTHING);
    set_game_mode(game, QUIT);

    /* return SDL_APP_CONTINUE; */
  }

  rerender_screen(game);

  bool has_food_been_eaten = has_just_eaten_food(snake, get_food_location(game));
  if (has_food_been_eaten) {
    change_food_location(game);

    // Maybe this should be after the if condition?
    /* rerender_snake(snake, current_movement); */
    rerender_screen(game);
  }

  /* Coordinates* snake_head = get_head(snake); */

  // Reset
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // TODO(ofer987): change back to const
  const float scale = 32.0f;
  SDL_SetRenderScale(renderer, scale, scale);

  // Render screen
  Coordinates* tile = get_screen(game);
  Uint8 red, green, blue;
  /* SDL_FRect* my_frects = malloc((HORIZONTAL_TILES_COUNT) * (VERTICAL_TILES_COUNT) * sizeof(SDL_FRect)); */
  SDL_FRect my_frects[HORIZONTAL_TILES_COUNT * VERTICAL_TILES_COUNT];
  SDL_FRect* snake_head_rect = NULL;

  size_t index = 0;
  while (tile != NULL) {
    my_frects[index].x = tile->x + SHIFT_WIDTH_SCREEN;
    my_frects[index].y = tile->y + SHIFT_HEIGHT_SCREEN;
    my_frects[index].w = 1;
    my_frects[index].h = 1;
    /* my_frect = {.x = tile->x * 10, .y = tile->y * 10, .w = WIDTH, .h = HEIGHT}; */

    switch (tile->type) {
      case AVAILABLE:
        red = 0;
        blue = 0;
        green = 0;
        break;
      case USED_BY_SNAKE_HEAD:
        snake_head_rect = &my_frects[index];

        red = 255;
        blue = 0;
        green = 0;
        break;
      case USED_BY_SNAKE_TAIL:
        red = 0;
        blue = 255;
        green = 0;
        break;
      case USED_BY_FOOD:
        red = 0;
        blue = 0;
        green = 255;
        break;
      default:
        red = 127;
        blue = 127;
        green = 127;
        break;
    }

    SDL_SetRenderDrawColor(renderer, red, blue, green, 255);
    SDL_RenderFillRect(renderer, &my_frects[index]);

    /* SDL_RenderFillRect(renderer, &my_frects[index]); */
    /* SDL_RenderPresent(renderer); */
    tile = tile->next;
    index += 1;
  }

  // Render Snake Head
  Coordinates* snake_head = get_snake_head(snake);
  red = 255;
  blue = 0;
  green = 0;
  SDL_FRect snake_head_frect;
  SDL_SetRenderDrawColor(renderer, red, blue, green, 255);
  SDL_RenderFillRect(renderer, &snake_head_frect);

  red = 95;
  blue = 74;
  green = 201;

  Coordinates* top_left_corner = get_top_left_corner(game);
  Coordinates* bottom_left_corner = get_bottom_left_corner(game);
  Coordinates* top_right_corner = get_top_right_corner(game);
  Coordinates* bottom_right_corner = get_bottom_right_corner(game);

  SDL_FRect top_border_frects[HORIZONTAL_TILES_COUNT];
  for (size_t x = 0; x < top_right_corner->x; x += 1) {
    top_border_frects[x].x = (float)x + SHIFT_WIDTH_SCREEN;
    top_border_frects[x].y = 0;
    top_border_frects[x].w = 1;
    top_border_frects[x].h = 1;

    SDL_SetRenderDrawColor(renderer, red, blue, green, 255);
    SDL_RenderFillRect(renderer, &top_border_frects[x]);
  }

  SDL_FRect left_border_frects[VERTICAL_TILES_COUNT];

  for (size_t y = 0; y < bottom_left_corner->y; y += 1) {
    left_border_frects[y].x = 0;
    left_border_frects[y].y = (float)y + SHIFT_HEIGHT_SCREEN;
    left_border_frects[y].w = 1;
    left_border_frects[y].h = 1;

    SDL_SetRenderDrawColor(renderer, red, blue, green, 255);
    SDL_RenderFillRect(renderer, &left_border_frects[y]);
  }

  SDL_FRect bottom_border_frects[HORIZONTAL_TILES_COUNT];
  for (size_t x = 0; x < bottom_right_corner->x; x += 1) {
    bottom_border_frects[x].x = (float)x + SHIFT_WIDTH_SCREEN;
    bottom_border_frects[x].y = (float)VERTICAL_TILES_COUNT + SHIFT_HEIGHT_SCREEN;
    bottom_border_frects[x].w = 1;
    bottom_border_frects[x].h = 1;

    SDL_SetRenderDrawColor(renderer, red, blue, green, 255);
    SDL_RenderFillRect(renderer, &bottom_border_frects[x]);
  }

  SDL_FRect right_border_frects[VERTICAL_TILES_COUNT];

  for (size_t y = 0; y < bottom_right_corner->y; y += 1) {
    right_border_frects[y].x = (float)HORIZONTAL_TILES_COUNT + SHIFT_WIDTH_SCREEN;
    right_border_frects[y].y = (float)y + SHIFT_HEIGHT_SCREEN;
    right_border_frects[y].w = 1;
    right_border_frects[y].h = 1;

    SDL_SetRenderDrawColor(renderer, red, blue, green, 255);
    SDL_RenderFillRect(renderer, &right_border_frects[y]);
  }

  SDL_FRect top_left_corner_border;
  top_left_corner_border.x = 0;
  top_left_corner_border.y = 0;
  top_left_corner_border.w = 1;
  top_left_corner_border.h = 1;
  SDL_RenderFillRect(renderer, &top_left_corner_border);

  SDL_FRect top_right_corner_border;
  top_right_corner_border.x = (float)HORIZONTAL_TILES_COUNT + SHIFT_WIDTH_SCREEN;
  top_right_corner_border.y = 0;
  top_right_corner_border.w = 1;
  top_right_corner_border.h = 1;
  SDL_RenderFillRect(renderer, &top_right_corner_border);

  SDL_FRect bottom_left_corner_border;
  bottom_left_corner_border.x = 0;
  bottom_left_corner_border.y = (float)VERTICAL_TILES_COUNT + SHIFT_HEIGHT_SCREEN;
  bottom_left_corner_border.w = 1;
  bottom_left_corner_border.h = 1;
  SDL_RenderFillRect(renderer, &bottom_left_corner_border);

  SDL_FRect bottom_right_corner_border;
  bottom_right_corner_border.x = bottom_right_corner_border.x = (float)HORIZONTAL_TILES_COUNT + SHIFT_WIDTH_SCREEN;
  bottom_right_corner_border.y = (float)VERTICAL_TILES_COUNT + SHIFT_HEIGHT_SCREEN;
  bottom_right_corner_border.w = 1;
  bottom_right_corner_border.h = 1;
  SDL_RenderFillRect(renderer, &bottom_right_corner_border);

  /* SDL_RenderPresent(renderer); */
  /* for (size_t i = 0; i < tiles_count; i += 1) { */
  /*   enum TILE_TYPES type = get_tile_type(i); */
  /*  */
  /*   switch (expression) {} */
  /* } */
  /* frect.x = snake->head->x; */
  /* frect.y = snake->head->y; */
  /* snake->head->x = frect.x; */
  /* snake->head->y = frect.y; */

  /* const char* message = "Hello World!"; */
  /* int w = 0, h = 0; */
  /* float x, y; */

  /* Center the message and scale it up */
  /* SDL_GetRenderOutputSize(renderer, &w, &h); */
  /* x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2; */
  /* y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2; */

  /* const SDL_Rect rect = {.x = 50, .y = 100, .w = 20, .h = 30}; */
  /* SDL_RenderRect(renderer, &frect); */

  /* if (snake_head_rect != NULL) { */
  /*   char rectange_x_location[4], rectange_y_location[4]; */
  /*   SDL_itoa(snake_head_rect->x, rectange_x_location, 10); */
  /*   SDL_itoa(snake_head_rect->y, rectange_y_location, 10); */
  /*   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); */
  /*  */
  /*   #<{(| scale = 4.0f; |)}># */
  /*   #<{(| SDL_SetRenderScale(renderer, scale, scale); |)}># */
  /*   SDL_RenderDebugText(renderer, 0, 0, rectange_x_location); */
  /*   SDL_RenderDebugText(renderer, 0, 10, rectange_y_location); */
  /* } */
  size_t score = get_snake_length(snake) - 1;
  render_text(score);
  if (get_game_mode(game) == QUIT) {
    render_failure();
  };

  SDL_RenderPresent(renderer);
  /* free(my_frects); */
  return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void
SDL_AppQuit(void* appstate, SDL_AppResult result) {}

