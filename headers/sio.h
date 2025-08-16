#ifndef _SIO_H
#define _SIO_H

#include <stdbool.h>

/* 
 * Raw SIO Interface to channel A
 */

// Fetch a single character from the serial input.
// Blocks until a character is available.
extern char chin();

// Output a single character to the serial output.
// Blocks until output channel is ready
extern void chout(char c);

// returns true if a key was pressed
extern bool cstat();

#endif //_SIO_H

