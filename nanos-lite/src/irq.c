#include "common.h"

static _RegSet* do_event(_Event e, _RegSet* r) {
  printf("111");
  switch (e.event) {
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
