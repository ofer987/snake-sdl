/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game.h"

static size_t GAME_WIDTH = 220;
static size_t GAME_HEIGHT = 340;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static float right_movement_span = 0;
static SDL_FRect frect = {.x = 50, .y = 100, .w = 10, .h = 10};

static Uint64 framerate_time = 0.0f;

/* enum MOVEMENTS { LEFT, UP, RIGHT, DOWN }; */
static enum MOVEMENTS movement;
static struct game_struct* game;
static struct snake_struct* snake;

/* This function runs once at startup. */
SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[]) {
  /* Create the window */
  if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
    SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  game = init_game(GAME_WIDTH, GAME_HEIGHT);
  snake = game->snake;
  frect.x = snake->head->x;
  frect.y = snake->head->y;

  return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_Q) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }

  // Move to the right
  if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_L) {
    game->snake->current_movement = RIGHT;
  }

  // Move up
  if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_K) {
    game->snake->current_movement = UP;
  }

  // Move left
  if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_H) {
    game->snake->current_movement = LEFT;
  }

  // Move down
  if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_J) {
    game->snake->current_movement = DOWN;
  }

  return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate) {
  Uint64 new_framerate_time = SDL_GetTicks();

  if (new_framerate_time < framerate_time + 200) {
    return SDL_APP_CONTINUE;
  }

  framerate_time = new_framerate_time;

  /* SDL_FRect frect = {.x = snake->head->x, .y = snake->head->y, .w = 10, .h = 10}; */

  switch (snake->current_movement) {
    case LEFT:
      if (snake->head->x <= 0) {
        break;
      }

      snake->head->x -= 10.0;
      break;
    case UP:
      if (snake->head->y <= 0) {
        break;
      }

      snake->head->y -= 10.0;
      break;
    case RIGHT:
      if (snake->head->x >= 340) {
        break;
      }

      snake->head->x += 10.0;
      break;
    case DOWN:
      if (snake->head->y >= (600 - 200) / 2.0) {
        break;
      }

      snake->head->y += 10.0;
    default: break;
  }

  frect.x = snake->head->x;
  frect.y = snake->head->y;
  /* snake->head->x = frect.x; */
  /* snake->head->y = frect.y; */

  const char* message = "Hello World!";
  int w = 0, h = 0;
  float x, y;
  const float scale = 4.0f;

  /* Center the message and scale it up */
  SDL_GetRenderOutputSize(renderer, &w, &h);
  SDL_SetRenderScale(renderer, scale, scale);
  x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
  y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

  /* const SDL_Rect rect = {.x = 50, .y = 100, .w = 20, .h = 30}; */
  /* SDL_RenderRect(renderer, &frect); */

  char rectange_x_location[4], rectange_y_location[4];
  SDL_itoa(frect.x, rectange_x_location, 10);
  SDL_itoa(frect.y, rectange_y_location, 10);
  /* Draw the message */
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugText(renderer, x, y, message);
  SDL_RenderFillRect(renderer, &frect);
  SDL_RenderDebugText(renderer, 0, 0, rectange_x_location);
  SDL_RenderDebugText(renderer, 0, 10, rectange_y_location);
  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void
SDL_AppQuit(void* appstate, SDL_AppResult result) {}

