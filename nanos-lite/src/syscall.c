#include "common.h"
#include "syscall.h"



_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  printf("a[0]:%d", a[0]);
  printf("SYS_none:%d\n", SYS_none);
  printf("SYS_open:%d\n", SYS_open);
  printf("SYS_read:%d\n", SYS_read);
  printf("SYS_write:%d\n", SYS_write);
  printf("SYS_exit:%d\n", SYS_exit);
  printf("SYS_kill:%d\n", SYS_kill);
  printf("SYS_getpid:%d\n", SYS_getpid);
  printf("SYS_close:%d\n", SYS_close);
  printf("SYS_lseek:%d\n", SYS_lseek);
  printf("SYS_brk:%d\n", SYS_brk);
  printf("SYS_fstat:%d\n", SYS_fstat);
  printf("SYS_time:%d\n", SYS_time);
  printf("SYS_signal:%d\n", SYS_signal);
  printf("SYS_execve:%d\n", SYS_execve);
  printf("SYS_fork:%d\n", SYS_fork);
  printf("SYS_link:%d\n", SYS_link);
  printf("SYS_unlink:%d\n", SYS_unlink);
  printf("SYS_wait:%d\n", SYS_wait);
  printf("SYS_times:%d\n", SYS_times);
  printf("SYS_gettimeofday:%d\n", SYS_gettimeofday);
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
