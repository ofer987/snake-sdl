#ifndef SNAKE_SDL_SNAKE_SDL_CONSTANTS_H_
#define SNAKE_SDL_SNAKE_SDL_CONSTANTS_H_

#define RESOLUTION_WIDTH       580
#define RESOLUTION_HEIGHT      680

#define MAZE_WIDTH             160
#define MAZE_HEIGHT            160
#define TEXT_HEIGHT_SHIFT      130

#define WIDTH                  10
#define HEIGHT                 10

#define HORIZONTAL_TILES_COUNT MAZE_WIDTH / WIDTH
#define VERTICAL_TILES_COUNT   MAZE_HEIGHT / HEIGHT

#endif // SNAKE_SDL_SNAKE_SDL_CONSTANTS_H_
       /*
 * (800 - 100) / 2 = 700 / 2 + 350
 * (600 - x) / 2 = (600 - x) / 2 = 220
 * x = 160
 */
