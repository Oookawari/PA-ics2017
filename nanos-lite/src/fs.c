#include "fs.h"

void ramdisk_read(void *buf, off_t offset, size_t len);
void ramdisk_write(const void *buf, off_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))
/*
void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = 4 * _screen.width * _screen.height;
  file_table[FD_FB].open_offset = 0;
}

int fs_open(const char *pathname, int flags, int mode) {
  for(int i = 0; i < NR_FILES; i++) {
    if(strcmp(pathname, file_table[i].name) == 0){
      //初始化
      file_table[i].open_offset = 0;
      return i;
    }
  }
  assert(0);//not found
  return -1;
}

void dispinfo_read(void *buf, off_t offset, size_t len);
size_t events_read(void *buf, size_t len);

ssize_t fs_read(int fd, void *buf, size_t len) {
  Finfo finfo = file_table[fd];
  switch(fd){
    case FD_DISPINFO:
      dispinfo_read(buf, finfo.open_offset, len);
      break;
    case FD_EVENTS:
      return events_read(buf, len);
    default:
      if(finfo.size - finfo.open_offset <= len) len = finfo.size - finfo.open_offset;
      ramdisk_read(buf, finfo.disk_offset + finfo.open_offset, len);
      file_table[fd].open_offset += len;
      return len;
  }
  return len;
}

void fb_write(const void *buf, off_t offset, size_t len);

ssize_t fs_write(int fd, const void *buf, size_t len) {
  Finfo finfo = file_table[fd];
  char *buffer = (char*) buf;
  switch(fd){
    case FD_STDOUT: 
    case FD_STDERR:
      for(int i = 0; i < len; i++) 
        _putc(buffer[i]);
      return len;
    case FD_FB:
      fb_write(buf, finfo.open_offset, len);
      break;
    default:
      if(finfo.size - finfo.open_offset <= len) 
        len = finfo.size - finfo.open_offset;
      ramdisk_write(buf, finfo.disk_offset + finfo.open_offset, len);
      file_table[fd].open_offset += len;
      return len;
  }
  return -1;
}

int fs_close(int fd) {
  return 0;
}

size_t fs_filesz(int fd){
  return file_table[fd].size;
}


off_t fs_lseek(int fd, off_t offset, int whence){
  Finfo finfo = file_table[fd];
  switch(whence){
    case SEEK_SET:
      file_table[fd].open_offset = offset;
      return file_table[fd].open_offset;
    case SEEK_CUR:
      file_table[fd].open_offset += offset;
      return file_table[fd].open_offset;
    case SEEK_END:
      file_table[fd].open_offset = finfo.size + offset;
      return file_table[fd].open_offset;
     default: return -1;
  }
}*/

void init_fs() {
    // TODO: initialize the size of /dev/fb
    file_table[FD_FB].size = _screen.width * _screen.height * sizeof(uint32_t);
    file_table[FD_FB].open_offset = 0;
}

int fs_open(const char *pathname, int flags, int mode){
    int fd;
    for(fd = 0; fd < NR_FILES; ++fd){
        if(strcmp(pathname, file_table[fd].name) == 0){
            break;
        }
    }
    if(fd >= NR_FILES) panic("file not found!");
    file_table[fd].open_offset = 0;
    Log("OPEN [%d] %s", fd, pathname);
    return fd;
}

void dispinfo_read(void *buf, off_t offset, size_t len);
void ramdisk_read(void *buf, off_t offset, size_t len);
size_t events_read(void *buf, size_t len);
ssize_t fs_read(int fd, void *buf, size_t len){
    Finfo *fp = &file_table[fd];
    
    ssize_t delta_len = fp->size - fp->open_offset;
    ssize_t write_len = delta_len < len?delta_len:len;

    switch(fd){
        case FD_STDOUT: case FD_STDERR:
            return -1;
        case FD_DISPINFO:
            dispinfo_read(buf, fp->open_offset, len);
            break;
        case FD_EVENTS:
            return events_read(buf, len);
        default:
            if(fd < 6 || fd >= NR_FILES) return -1;
            ramdisk_read(buf, fp->disk_offset + fp->open_offset, write_len);
            break;
    }

    fp->open_offset += write_len;
    // fs_lseek()
    return write_len;
}

void fb_write(const void *buf, off_t offset, size_t len);
void ramdisk_write(const void *buf, off_t offset, size_t len);
ssize_t fs_write(int fd, uint8_t *buf, size_t len){
    
    Finfo *fp = &file_table[fd];

    ssize_t delta_len = fp->size - fp->open_offset;
    ssize_t write_len = delta_len < len?delta_len:len;

    size_t i = 0;
    switch(fd){
        //case FD_STDIN: return -1;
        
        case FD_STDOUT: case FD_STDERR:
            while(i++ < len) _putc(*buf++);
            return len;
        
        case FD_FB:
            fb_write(buf, fp->open_offset, len);
            break;

        default:
            if(fd < 6 || fd >= NR_FILES) return -1;
            ramdisk_write(buf, fp->disk_offset + fp->open_offset, write_len);
            break;
    }

    fp->open_offset += write_len;
    return write_len;
}

off_t fs_lseek(int fd, off_t offset, int whence){
    
    Finfo *fp = &file_table[fd];

    if(fd >= NR_FILES) return 0;

    switch(whence){
        case SEEK_SET:
            break;
        case SEEK_CUR:
            offset = fp->open_offset + offset;
            break;
        case SEEK_END:
            offset = fp->size + offset;
            break;
        default: return -1;
    }
    if(offset < 0 || offset > fp->size) return -1;
    fp->open_offset = offset;
    return fp->open_offset;
}

int fs_close(int fd){
    return 0;
}

size_t fs_filesz(int fd){
    return file_table[fd].size;
}

