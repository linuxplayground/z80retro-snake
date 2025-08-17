#ifndef _TMS99XX_H
#define _TMS99XX_H

#include <stdint.h>

/*
 * Initialise the TMS99xx in multicolor mode. - Sets up the vdp with interrupts
 * enabled, 16x16 Sprites and maginifcation off. - The color table is not used.
 * Other VRAM addresses are: 
 *  - Name table: 0x1400 
 *  - Pattern table: 0x0800 
 *  - Sprite attribute table: 0x1000 
 *  - Sprite pattern table: 0x000 
 * Init the sprites and setting the first one to disabled - thus disabling the rest.
 * Init the name table with with 6 groups of 4 sets of 32 values.  The values
 * increase by 32 each group. This allows for pixel plotting using the `vmc_plot_xy`
 * function described in the `vdp.h` header file.
 */
extern void tms_mcinit();

/*
 * Set the write address register
 */
extern void tms_w_addr(uint16_t addr);

/*
 * Set the read address register
 */
extern void tms_r_addr(uint16_t addr);

/*
 * Copy 0x600 bytes from the buffer into the pattern table in VRAM
 */
extern void tms_flushmc(char *buf);

/*
 * Polls the VDP status register for the VSYNC interrupt.  On the Z80-Retro!
 * that I have, this is done via a "proxy" pin on the Joystick 0 port.  If you
 * don't have this setup, and want to poll the VDP directly, like you would on
 * the Z80-Nouveau then you will need to alter the assembly instructions in the
 * `_tms_wait` routine in `asm/tms99xx.s`
 */
extern void tms_wait();

#endif //_TMS99XX_H
