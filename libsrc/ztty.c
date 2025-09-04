#include "cpm.h"
#include "ztty.h"


void zputs(char *s) {
  do {
    conout(*s);
  } while (*s++ != 0);
}
