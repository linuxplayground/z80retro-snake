#ifndef _TMS99XX_H
#define _TMS99XX_H

#include <stdint.h>

extern void tms_mcinit();
extern void tms_w_addr(uint16_t addr);
extern void tms_r_addr(uint16_t addr);
extern void tms_flushmc(char *buf);
extern void tms_wait();

#endif //_TMS99XX_H
