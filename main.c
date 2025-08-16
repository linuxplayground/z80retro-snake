#include "sio.h"
#include "ztty.h"
#include "tms99xx.h"

char fb[0x600] = {0x11};

void greeter(char *name)
{
  zputs("\nHello, ");
  zputs(name);
  chout('\n');
}

int main() {
  char c;
  tms_mcinit();
  greeter("Dave");
  zputs("Press a key to quit.\n");
  while (!cstat())
    ;
  c = chin();

  fb[0] = 0x34;
  tms_flushmc(fb);

  zputs("Exiting...");
  return 0;
}
