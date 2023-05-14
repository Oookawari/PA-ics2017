#include "common.h"
#include "syscall.h"



_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  printf("a[0]:%d", a[0]);
  printf("SYS_exit:%d\n", SYS_exit);
  printf("SYS_open:%d\n", SYS_open);
  printf("SYS_read:%d\n", SYS_read);
  printf("SYS_exit:%d\n", SYS_exit);
  printf("SYS_exit:%d\n", SYS_exit);
  printf("SYS_exit:%d\n", SYS_exit);
  switch (a[0]) {
    case SYS_none:
      r->eax = 1; 
      break;
    case SYS_exit:
      _halt(a[1]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
