#ifndef _RAND_H
#define _RAND_H
#include <stdint.h>

/*
 * Pseudo random number generator from Fuzix.
 *
 * Initialise the random number generator with a seed.
 */
void srand(uint16_t seed);

/*
 * Return a uint16_t pseudo random number.
 */
uint16_t rand();

#endif// _RAND_H

