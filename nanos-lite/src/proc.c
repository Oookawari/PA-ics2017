#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  //_switch(&pcb[i].as);
  //current = &pcb[i];
  //((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

int pal_times = 0;

_RegSet* schedule(_RegSet *prev) {
  current->tf = prev;
  //current = &pcb[0];
  
  if(pal_times < 100) {
    pal_times++;

    current = &pcb[0];
  }
  else {current = &pcb[1];pal_times = 0;
    Log("调度至:hello程序\n");
  }
  //_switch(&pcb[0].as);
  _switch(&current->as);
  return current->tf;

}
