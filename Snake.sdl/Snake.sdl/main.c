/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#include <stdint.h>
#include <stdlib.h>
#include "./types.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "./constants.h"
#include "./coordinates.h"
#include "./game.h"
#include "./snake.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static Uint64 MILLISECONDS_PER_FRAME = 10;
static Uint64 MILLISECONDS_PER_SCREEN_RENDER = 100;

static uint64_t framerate_time = 0.0f;
static uint64_t rendered_movement_frames = 0;

static Game* game;
static bool movement_changed = false;

#ifdef DEBUG
#define WINDOW_SCREEN_MODE SDL_WINDOW_FULLSCREEN
#else
#define WINDOW_SCREEN_MODE SDL_WINDOW_MAXIMIZED
#endif

/* This function runs once at startup. */
SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[]) {
  srandom((unsigned)time(NULL));

  SDL_HideCursor();

  /* Create the window */
  if (!SDL_CreateWindowAndRenderer("Blockedly Blocked-Block Snake", RESOLUTION_WIDTH, RESOLUTION_HEIGHT,
                                   WINDOW_SCREEN_MODE | SDL_WINDOW_INPUT_FOCUS, &window, &renderer)) {
    SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  game = init_game(HORIZONTAL_TILES_COUNT, VERTICAL_TILES_COUNT);

  // There are 1_000_000_000 nanoseconds in a second
  /* ns_per_frame = 1000000000 / fps; */

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
      case SDLK_Q:
        set_game_mode(game, QUIT);

        return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
      case SDLK_R:
        destroy_game(game);

        // Set to NULL for best-practice
        game = NULL;
        game = init_game(HORIZONTAL_TILES_COUNT, VERTICAL_TILES_COUNT);

        return SDL_APP_CONTINUE;
      case SDLK_P:
        set_current_movement(game, NOTHING);
        set_game_mode(game, PAUSE);

        return SDL_APP_CONTINUE;
    }

    // Return early
    // If the movement-change has not been rendered
    if (movement_changed) {
      return SDL_APP_CONTINUE;
    }

    if (game_mode == START || game_mode == CONTINUE || game_mode == PAUSE) {
      set_game_mode(game, CONTINUE);

      switch (key) {
        case SDLK_LEFT:
          /* FALLTHROUGH */
        case SDLK_H:
          if (current_movement != RIGHT) {
            set_current_movement(game, LEFT);

            movement_changed = true;
          }

          break;
        case SDLK_UP:
          /* FALLTHROUGH */
        case SDLK_K:
          if (current_movement != DOWN) {
            set_current_movement(game, UP);

            movement_changed = true;
          }

          break;
        case SDLK_RIGHT:
          /* FALLTHROUGH */
        case SDLK_L:
          if (current_movement != LEFT) {
            set_current_movement(game, RIGHT);

            movement_changed = true;
          }

          break;
        case SDLK_DOWN:
          /* FALLTHROUGH */
        case SDLK_J:
          if (current_movement != UP) {
            set_current_movement(game, DOWN);

            movement_changed = true;
          }

          break;
      }

      // Cheat Codes
      switch (key) {
        case SDLK_D: set_cheat_codes(game, NO_DEATH); break;
        default: break;
      }
    }
  }

  return SDL_APP_CONTINUE;
}

void
render_text(size_t score) {
  const float scale = 2.0f;

  SDL_SetRenderScale(renderer, scale, scale);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugTextFormat(renderer, 0, MAZE_HEIGHT + TEXT_HEIGHT_SHIFT + 0, "Score: %zu", score);
}

void
render_keys(void) {
  const float scale = 2.0f;

  SDL_SetRenderScale(renderer, scale, scale);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugText(renderer, 0, MAZE_HEIGHT + TEXT_HEIGHT_SHIFT + 10, "Press: (Q)uit | (R)estart | (P)ause");
}

void
render_lost_keys(void) {
  const float scale = 2.0f;

  SDL_SetRenderScale(renderer, scale, scale);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugText(renderer, 0, MAZE_HEIGHT + TEXT_HEIGHT_SHIFT + 10, "Press: (Q)uit | (R)estart");
}

void
render_pause(void) {
  const float scale = 2.0f;

  SDL_SetRenderScale(renderer, scale, scale);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugText(renderer, 0, MAZE_HEIGHT + TEXT_HEIGHT_SHIFT + 20, "Game is Paused. Any key to continue");
}

void
render_lost(void) {
  const float scale = 2.0f;

  SDL_SetRenderScale(renderer, scale, scale);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugText(renderer, 0, MAZE_HEIGHT + TEXT_HEIGHT_SHIFT + 20, "You have lost!");
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate) {
  Snake* snake = get_snake_location(game);

  // Move the snake
  bool is_snake_rerendered = false;
  uint64_t new_rendered_movement_frames = SDL_GetTicks();
  if (new_rendered_movement_frames >= rendered_movement_frames + MILLISECONDS_PER_SCREEN_RENDER) {
    enum MOVEMENTS current_movement = get_current_movement(game);

    rerender_snake(snake, current_movement, HORIZONTAL_TILES_COUNT, VERTICAL_TILES_COUNT);
    is_snake_rerendered = true;
    rendered_movement_frames = new_rendered_movement_frames;
  }

  rerender_screen(game);
  Coordinates** screen = get_screen(game);

  if (get_game_mode(game) == QUIT) {
    set_current_movement(game, NOTHING);
  }

  bool is_there_a_collision = is_snake_rerendered && has_snaked_collided(snake, screen);

  if (is_there_a_collision) {
    set_current_movement(game, NOTHING);

    if (!no_death(game)) {
      set_game_mode(game, QUIT);
    }
  }

  bool has_food_been_eaten = is_snake_rerendered && has_just_eaten_food(snake, get_food_location(game));
  if (has_food_been_eaten) {
    change_food_location(game);

    rerender_screen(game);
    screen = get_screen(game);
  }

  // Reset
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  const float scale = 32.0f;
  SDL_SetRenderScale(renderer, scale, scale);

  // Render screen
  Uint8 red = 0, green = 0, blue = 0;
  SDL_FRect my_frects[(HORIZONTAL_TILES_COUNT + 2) * (VERTICAL_TILES_COUNT + 2)];
  SDL_FRect* snake_head_rect = NULL;

  size_t index = 0;

  Coordinates* screen_coordinates = screen[index];
  while (screen_coordinates != NULL) {
    my_frects[index].x = screen_coordinates->x;
    my_frects[index].y = screen_coordinates->y;
    my_frects[index].w = 1;
    my_frects[index].h = 1;

    switch (screen_coordinates->type) {
      case AVAILABLE:
        red = 0;
        green = 0;
        blue = 0;

        break;
      case USED_BY_SNAKE_HEAD:
        snake_head_rect = &my_frects[index];

        red = 0;
        green = 0;
        blue = 255;

        break;
      case USED_BY_SNAKE_TAIL: break;
      case USED_BY_FOOD:
        red = 0;
        green = 255;
        blue = 0;

        break;
      case USED_BY_TOP_LEFT_BORDER:
        /* FALLTHROUGH */
      case USED_BY_TOP_RIGHT_BORDER:
        /* FALLTHROUGH */
      case USED_BY_BOTTOM_RIGHT_BORDER:
        /* FALLTHROUGH */
      case USED_BY_BOTTOM_LEFT_BORDER:
        /* FALLTHROUGH */
      case USED_BY_TOP_BORDER:
        /* FALLTHROUGH */
      case USED_BY_RIGHT_BORDER:
        /* FALLTHROUGH */
      case USED_BY_BOTTOM_BORDER:
        /* FALLTHROUGH */
      case USED_BY_LEFT_BORDER:
        red = 95;
        green = 74;
        blue = 201;

        break;
      default:
        // i.e., collision
        red = 255;
        green = 0;
        blue = 0;

        break;
    }

    // Draw tail separately, so that its colour fades
    // from light Blue to white
    if (screen_coordinates->type != USED_BY_SNAKE_TAIL) {
      SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
      SDL_RenderFillRect(renderer, &my_frects[index]);
    }

    index += 1;
    screen_coordinates = get_screen_coordinate(game, index);
  }

  size_t score = get_snake_length(snake) - 1;
  size_t score_index = 0;
  Coordinates* snake_tail = get_snake_head(snake)->next;
  while (snake_tail != NULL) {
    index = x_y_to_index(snake_tail->x, snake_tail->y);

    screen_coordinates = screen[index];

    my_frects[index].x = snake_tail->x;
    my_frects[index].y = snake_tail->y;
    my_frects[index].w = 1;
    my_frects[index].h = 1;

    // Snake tail fades from light blue to white
    float float_red = ((float)score_index) / score;
    switch (screen_coordinates->type) {
      case USED_BY_SNAKE_TAIL:
        red = 255 * float_red;
        green = 255;
        blue = 255;

        break;
      default:
        // i.e., collision
        red = 255;
        green = 0;
        blue = 0;

        break;
    }

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    SDL_RenderFillRect(renderer, &my_frects[index]);

    score_index += 1;
    snake_tail = snake_tail->next;
  }

  render_text(score);

  enum GAME_MODES game_mode = get_game_mode(game);
  switch (game_mode) {
    case QUIT:
      render_lost();
      render_lost_keys();

      break;
    case PAUSE:
      render_pause();

      /* FALLTHROUGH */
    default: render_keys(); break;
  }

  SDL_RenderPresent(renderer);
  movement_changed = false;

  Uint64 new_framerate_time = SDL_GetTicks();

  if (new_framerate_time < framerate_time + MILLISECONDS_PER_FRAME) {
    SDL_Delay(framerate_time + MILLISECONDS_PER_FRAME - new_framerate_time);
  }

  framerate_time = new_framerate_time;

  return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void
SDL_AppQuit(void* appstate, SDL_AppResult result) {}

