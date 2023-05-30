#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

void ramdisk_read(void *buf, off_t offset, size_t len);

size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  //size_t len = get_ramdisk_size();
  //ramdisk_read(DEFAULT_ENTRY, 0, len);
  //return (uintptr_t)DEFAULT_ENTRY;
  int fd = fs_open(filename, 0, 0);
  size_t file_size = fs_filesz(fd);
  fs_read(fd, DEFAULT_ENTRY, file_size);
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
