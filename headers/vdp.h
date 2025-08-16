#ifndef _VDP_H
#define _VDP_H

#include <stdbool.h>
#include <stdint.h>

#define VDP_NAME              0x1400
#define VDP_PATTERN           0x800
#define VDP_SPRITE_ATTRIBUTE  0x1000
#define VDP_SPRITE_PATTERN    0x0000


#define VDP_TRANSPARENT         0
#define VDP_BLACK               1
#define VDP_MEDIUM_GREEN        2
#define VDP_LIGHT_GREEN         3
#define VDP_DARK_BLUE           4
#define VDP_LIGHT_BLUE          5
#define VDP_DARK_RED            6
#define VDP_CYAN                7
#define VDP_MEDIUM_RED          8
#define VDP_LIGHT_RED           9
#define VDP_DARK_YELLOW         10
#define VDP_LIGHT_YELLOW        11
#define VDP_DARK_GREEN          12
#define VDP_MAGENTA             13
#define VDP_GRAY                14
#define VDP_WHITE               15


/* VDP Plot XY (Multicolor Mode)
 * Returns boolean if the colour being plotted is not black or transparent
 * and clashes with an existing colour in the frame buffer
 */
bool vmc_plot_xy(uint8_t x, uint8_t y, uint8_t c, char *buf);

#endif //_VDP_H
