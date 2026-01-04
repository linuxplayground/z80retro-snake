/*
 * Copyright 2025 David Latham
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "tms99xx.h"
#include "vdp.h"
#include "ztty.h"
#include "cpm.h"
#include "rand.h"

#include <stdbool.h>
#include <stdint.h>

#define SPEED   3   // How fast the snake move.  2 is quite fast, 3 is more sedate
#define GROWLEN 4   // How many segments does the snake grow by when it eats an apple

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

// Array of snake segment Point structs large enough to fill whole screen
Point points[0x600];

// Array of double pixel values represented in hex that hold the state of
// the screen.  This array is flushed to the VDP pattern table by `vmc_flush(*buf)`
char fb[0x600] = {0x11};

Point apple = {0};

// Initilize the snake and set it's first location.  Grow is set to 
Snake *init_snake() {
  Snake *snake;
  points[0].x = 31; // hardcoded centre of screen
  points[0].y = 24;
  snake->head = 0;  // head index at start of points array.
  snake->tail = 0;
  snake->grow = GROWLEN; // snake grows to 4 segments when game starts.
  return snake;
}

/*
 * Move the snake in direction given by global `dir`.
 * First calculate new xy and then if not crashed into a wall
 * set the next item in the points array to the new xy positions.
 * Update the snake head index to be that of the next item.
 * When selecting the next item, only do so when there is room in
 * the array.  Otherwise set it to the first item.
 * The tail index is only updated when the snake is NOT growing.
 * The same rules about the head index apply to the tail index.
 * This is how I have implimented a circular buffer.
 */
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

  // Check for wall collision
  if ((nx > 63) || (nx < 0) || (ny > 47) || (ny < 0))
    return true;  // True means crashed!

  // Handle circular buffer for head
  if (snake->head++ > (0x600 - sizeof(Point))) {
    snake->head = 0;
  }

  // Set the item xy with these new coordinates.
  points[snake->head].x = nx;
  points[snake->head].y = ny;

  // Deal with tail.  Only increment the tail index if the snake is not
  // growing and take care to roll round to start of array when at the end
  if (snake->grow == 0) {
    if (snake->tail++ > (0x600 - sizeof(Point))) {
      snake->tail = 0;
    }
  } else {
    snake->grow--;  // if snake is still growing, decrement the grow counter.
  }

  return false; // false means did NOT crash.
}

/*
 * Draws the snake head in DARK_RED and the TAIL in BLACK.
 */
bool draw_snake(Snake *snake) {
  if (snake->grow == 0) {   // If the snake is not growing, draw the tail in black.
    vmc_plot_xy(points[snake->tail].x, points[snake->tail].y, VDP_BLACK, fb);
  }
  /*
   * Return the boolan result of plotting the head.  `vmc_plot_xy()` returns true if
   * the position was already occupied. If that happens then the snake has collided with
   * itself or an apple.
   */
  return vmc_plot_xy(points[snake->head].x, points[snake->head].y, VDP_DARK_RED,
                     fb);
}

/*
 * Find an empty location for an apple.
 * This works by selecting a random xy position in the screen and drawing the
 * apple there. If the result of the plot was a collision, try again until the
 * result was not a collision.
 *
 * This will have the effect of some apples appearing on top of the snake body
 * but they can be ignored as there should always be a viable apple to target.
 */
void new_apple() {
  bool taken = true;
  uint8_t x, y;
  uint16_t offset;

  while (taken) {
    x = rand() % 64;
    y = rand() % 48;
    offset = (y * 64) + x;
    if (!vmc_plot_xy(x, y, VDP_LIGHT_GREEN, fb)) {
      //There was no collision so this must be a valid location for an apple.
      taken = false;
      apple.x = x;
      apple.y = y;
    }
  }
}

// Lifted straight out of Fuzix
// TODO: Should be a library function.
void *z80memset(void *dest, int data, uint16_t len)
{
  char *p = dest;
  char v = (char)data;

  while(len--)
    *p++ = v;
  return dest;
}

/* 
 * Main entrypoint of the application
 * Sets up a new snake, clears all the cells in the frame buffer
 * Sets the random seed.
 */
int main() {
  Snake *snake = init_snake();
  z80memset(fb, 0x11, 0x600);

  running = false;
  speed = SPEED;

  tms_mcinit();
  zputs("Press SPACE to start, WASD to move, ESC to quit.\r\n");
 
  // The seed is incremented in a tight loop until the user presses
  // SPACE to start.  This gives a convenient way to initialize the
  // pseudo random number generator.
  while (!running) {
    seed ++;
    if(conio() == ' ')
      running = true;
  }
  srand(seed);

  // Plot a new apple.  The apple might appear on the snake initial
  // position, but this will just have the effect of immediately growing
  // the snake and a new apple being placed in the game window.
  new_apple();
  zputs("\"How do ya like dem apples?\"\r\n"); // Good Will Hunting - 1997

  while (running) {
    // About as close to 60FPS as we can get.
    // Speed is used to regulate the number of frames between game updates.
    // User input is collected (and processed) every frame.
    tms_wait();
    speed--;

    // We don't allow changing direction back on ourselves.
    key = conio(); // get the key
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
    if (speed == 0 && running) {// Only update the game when the speed counter
                                // reaches zero
      if (move_snake(snake)) {
        running = false;
        zputs("You crashed into a wall.\r\n");
        break;
      }

      if (draw_snake(snake)) {
        // If draw_snake returned a collision, then check if it was an apple.
        if ((points[snake->head].x == apple.x) &&
            (points[snake->head].y == apple.y)) {
          snake->grow = GROWLEN;  // set the grow length 
          new_apple();            // find a new apple
        } else {
          // We only get here if we collided into something that was not an apple.
          zputs("You crashed into your tail.\r\n");
          running = false;
        }
      }
      // Repaint the screen
      tms_flushmc(fb);
      // Reset speed countdown
      speed = SPEED;
    }
  }

  zputs("Exiting...");
  // CP/M does not suppport exit codes, so this is superflous.  However, my
  // clangd linter REALLY wants the main function to be one that returns an
  // integer.  Rather than fight the linter...
  return 0;
}
