#include "common.h"
_RegSet* schedule(_RegSet *prev);
static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
    case _EVENT_IRQ_TIME:
      //Log("触发_EVENT_IRQ_TIME");
      return schedule(r);
      break;
    case _EVENT_SYSCALL:
      return do_syscall(r);
      break;
    case _EVENT_TRAP:
      //Log("触发EVENT_TRAP");
      return schedule(r);
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
