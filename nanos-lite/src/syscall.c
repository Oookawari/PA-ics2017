#include "common.h"
//#include "syscall.h"

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

enum {
  SYS_none,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_exit,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

#endif

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  printf("a[0] : %d", a[0]);
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
