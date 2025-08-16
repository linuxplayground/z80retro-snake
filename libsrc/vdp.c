#include "vdp.h"

/* VDP Plot XY (Multicolor Mode)
 * Returns boolean if the colour being plotted is not black or transparent
 * and clashes with an existing colour in the frame buffer
 */
bool vmc_plot_xy(uint8_t x, uint8_t y, uint8_t c, char *buf) {
  static uint8_t dot = 0;   // Existing double-pixel in frame buffer
  static char pix = 0;      // New value of double-pixel in frame buffer
  static uint16_t addr = 0; // Address offset into the framebuffer (index)
  char collide = false;     // True if collision detected
  addr = 8 * (x / 2) + y % 8 + 256 * (y / 8);
  dot = buf[addr]; // Existing double-pixel value

  /*
   * Depending on if we are dealing with the left or right (even or odd) pixel
   * in the double pixel. We mask and shift so we can find a collision and set
   * the new double-pixel We don't consider BLACK or TRANSPARENT as colours that
   * would result in a collision.
   */
  if (x & 1) // Odd columns
  {
    // -X
    if (((dot & 0x0F) > VDP_BLACK) && (c > VDP_BLACK)) {
      collide = true;
    }
    pix = (dot & 0xF0) | c;
  } else {
    // X-
    if ((c > VDP_BLACK) && (dot >> 4) > VDP_BLACK) {
      collide = true;
    }
    pix = (dot & 0x0F) | (c << 4);
  }
  buf[addr] = pix;
  return collide;
}


