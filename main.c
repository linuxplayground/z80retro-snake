#include "rand.h"
#include "sio.h"
#include "tms99xx.h"
#include "vdp.h"
#include "ztty.h"

#include <stdbool.h>
#include <stdint.h>

#define SPEED 3

enum e_dir { NORTH, EAST, SOUTH, WEST };

uint16_t seed;
uint8_t speed;
bool running;
uint8_t dir = EAST;
uint8_t key;

typedef struct s_point {
  uint8_t x;
  uint8_t y;
} Point;

typedef struct s_snake {
  uint16_t head; // index into points for the head
  uint16_t tail; // index into points for the tail
  uint8_t grow;  // number of segments to grow by
} Snake;

Point points[0x600] = {{0}};
char fb[0x600] = {0x11};
Point apple = {0};

Snake *init_snake() {
  Snake *snake;
  points[0].x = 31;
  points[0].y = 24;
  snake->head = 0;
  snake->tail = 0;
  snake->grow = 4;
  return snake;
}

bool move_snake(Snake *snake) {
  // snake->head is index into points where head is.
  int8_t nx = points[snake->head].x;
  int8_t ny = points[snake->head].y;

  switch (dir) {
  case EAST:
    nx++;
    break;
  case WEST:
    nx--;
    break;
  case NORTH:
    ny--;
    break;
  case SOUTH:
    ny++;
    break;
  default:
    break;
  }

  if ((nx > 63) || (nx < 0) || (ny > 47) || (ny < 0))
    return true;
  if (snake->head++ > (0x600 - sizeof(Point))) {
    snake->head = 0;
  }

  points[snake->head].x = nx;
  points[snake->head].y = ny;

  if (snake->grow == 0) {
    if (snake->tail++ > (0x600 - sizeof(Point))) {
      snake->tail = 0;
    }
  } else {
    snake->grow--;
  }

  return false;
}

bool draw_snake(Snake *snake) {
  if (snake->grow == 0) {
    vmc_plot_xy(points[snake->tail].x, points[snake->tail].y, VDP_BLACK, fb);
  }
  return vmc_plot_xy(points[snake->head].x, points[snake->head].y, VDP_DARK_RED,
                     fb);
}

void new_apple() {
  bool taken = true;
  uint8_t x, y;
  uint16_t offset;

  while (taken) {
    x = rand() % 64;
    y = rand() % 48;
    offset = y * 64 + x;
    if (fb[offset] != 0x11) {
      taken = false;
      apple.x = x;
      apple.y = y;
      vmc_plot_xy(x, y, VDP_LIGHT_GREEN, fb);
    }
  }
}

int main() {
  char c;
  Snake *snake = init_snake();

  running = false;
  speed = SPEED;

  tms_mcinit();
  zputs("Press SPACE to start, WASD to move, ESC to quit.\n");
  
  while (!running) {
    seed ++;
    if (cstat()) {
      if(chin() == ' ')
        running = true;
    }
  }
  srand(seed);

  new_apple();
  zputs("\"How do ya like dem apples?\"\n");

  while (running) {
    tms_wait();
    speed--;

    if (cstat()) {  // a key was pressed
      key = chin(); // get the key
      switch (key) {
      case 0x1b:
        running = false;
        break;
      case 'a':
        dir = (dir != EAST) ? WEST : dir;
        break;
      case 's':
        dir = (dir != NORTH) ? SOUTH : dir;
        break;
      case 'd':
        dir = (dir != WEST) ? EAST : dir;
        break;
      case 'w':
        dir = (dir != SOUTH) ? NORTH : dir;
        break;
      default:
        break;
      }
    }
    if (speed == 0 && running) {

      if (move_snake(snake)) {
        running = false;
        zputs("You crashed into a wall.\n");
        break;
      }

      if (draw_snake(snake)) {
        if ((points[snake->head].x == apple.x) &&
            (points[snake->head].y == apple.y)) {
          snake->grow = 4;
          new_apple();
        } else {
          zputs("You crashed into your tail.\n");
          running = false;
        }
      }

      tms_flushmc(fb);
      speed = SPEED;
    }
  }

  zputs("Exiting...");
  return 0;
}
