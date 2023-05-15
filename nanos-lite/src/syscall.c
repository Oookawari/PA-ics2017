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

int fs_open(const char *pathname, int flags, int mode);
ssize_t fs_read(int fd, void *buf,size_t len);
ssize_t fs_write(int fd, const void *buf, size_t len);
off_t fs_lseek(int fd, off_t offset, int whence);
int fs_close(int fd);
size_t fs_filesz(int fd);

static inline uintptr_t sys_write(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  uint8_t* buffer = (uint8_t*)buf;
  switch (fd)
  {
  case 1://stdout
  case 2://stderr
    for(int i = 0; i < len; i++) 
      _putc(buffer[i]);
    return len;
  default:
    break;
  }
   return -1;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  //printf("a[0] : %d\n", a[0]);
  switch (a[0]) {
    case SYS_none:
      r->eax = 1; 
      break;
    case SYS_write:
      {
      int rtv = sys_write(a[1], a[2], a[3]);
      r->eax = rtv;
      break;
      }
    case SYS_exit:
      _halt(a[1]);
      break;
    case SYS_brk:
      //a[2] = a[1];
      r->eax = 0;
      break;
    case SYS_open:  {
      int rtv =  fs_open((char *)a[1], a[2], a[3]);
      r->eax = rtv; 
      break;
    }
    case SYS_read:  {
      int rtv =  fs_read(a[1], (void *)a[2], a[3]); 
      r->eax = rtv;
      break;
    }
    case SYS_close: {
      int rtv =  fs_close(a[1]); 
      r->eax = rtv;
      break;
    }
    case SYS_lseek: {
      off_t rtv =  fs_lseek(a[1], a[2], a[3]);
      r->eax = rtv; 
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

