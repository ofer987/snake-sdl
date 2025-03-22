#ifndef SNAKE_SDL_SNAKE_SDL_CONSTANTS_H_
#define SNAKE_SDL_SNAKE_SDL_CONSTANTS_H_

#define RESOLUTION_WIDTH       800
#define RESOLUTION_HEIGHT      600

#define MAZE_WIDTH             350
#define MAZE_HEIGHT            220

#define WIDTH                  10
#define HEIGHT                 10

#define SHIFT_WIDTH_SCREEN     1
#define SHIFT_HEIGHT_SCREEN    1

#define HORIZONTAL_TILES_COUNT MAZE_WIDTH / WIDTH
#define VERTICAL_TILES_COUNT   MAZE_HEIGHT / HEIGHT

#endif // SNAKE_SDL_SNAKE_SDL_CONSTANTS_H_
       /*
 * (800 - 100) / 2 = 700 / 2 + 350
 * (600 - x) / 2 = (600 - x) / 2 = 220
 * x = 160
 */
