#ifndef _ZTTY_H
#define _ZTTY_H

/*
 * Helper functions for serial input / output
 *
 * Print a null terminated string to the serial console.  
 * Uses `chout` from `sio.h`.
 */
void zputs(char *s);

#endif //_ZTTY_H

