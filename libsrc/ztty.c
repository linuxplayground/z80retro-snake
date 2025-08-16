#include "sio.h"
#include "ztty.h"


void zputs(char *s) {
  do {
    chout(*s);
  } while (*s++ != 0);
}
