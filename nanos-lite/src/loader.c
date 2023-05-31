#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x8048000)
#ifndef PGSIZE
#define PGSIZE 4096
#endif

void* new_page(void);

void ramdisk_read(void *buf, off_t offset, size_t len);

size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  
  //size_t len = get_ramdisk_size();
  //ramdisk_read(DEFAULT_ENTRY, 0, len);
  //return (uintptr_t)DEFAULT_ENTRY;
  int fd = fs_open(filename, 0, 0);
  Log("进入loader() 加载文件名%s",filename);
  size_t file_size = fs_filesz(fd);
  //fs_read(fd, DEFAULT_ENTRY, file_size);
  //fs_close(fd);
  void* paddr;
  for(int i = 0; i <= file_size / PGSIZE; i++) {
    paddr = new_page();
    if(i == file_size / PGSIZE){
      fs_read(fd, paddr, file_size % PGSIZE);
    }
    else{
      fs_read(fd, paddr, PGSIZE);
    }
    _map(as, DEFAULT_ENTRY + i * PGSIZE, paddr);
  }
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
