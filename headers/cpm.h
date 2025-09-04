#ifndef _CPM_H
#define _CPM_H

#include <stdbool.h>

/*
 * CPM access to raw io
 *
 * Fetch a single character from the serial input.
 * Zero if no char waiting
 */
extern char conio();
extern void conout(char c);

#endif //_SIO_H

