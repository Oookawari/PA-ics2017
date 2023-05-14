#ifndef __COMMON_H__
#define __COMMON_H__

#include <am.h>
#include <klib.h>
#include "debug.h"

typedef char bool;
#define true 1
#define false 0

#endif

_RegSet* do_syscall(_RegSet *r);

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