#include "common.h"

#define KEYDOWN_MASK 0x8000

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};
unsigned long _uptime();
int _read_key();

size_t events_read(void *buf, size_t len) {
  char* buffer = (char* )buf;
  int key_evt = _read_key();
  if(key_evt == _KEY_NONE){
    char evt_str[len];
    long time = _uptime();
    sprintf(evt_str, "t %ld\n", time);
    for(int i = 0; i < len; i++) {  
      buffer[i] = evt_str[i];
    }
  }
  else {
    char evt_str[len];
    sprintf(evt_str, "ku %s\n", keyname[key_evt]);
    for(int i = 0; i < len; i++) {  
      buffer[i] = evt_str[i];
    }
    if((key_evt & KEYDOWN_MASK) && len >= 2){
      buffer[1] = 'd';
    }
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  char* buffer = (char* )buf;
  for(int i = 0; i < len; i++) {
    buffer[i] = dispinfo[offset + i];
  }

}
void fb_write(const void *buf, off_t offset, size_t len) {
  offset = offset / 4;
  int x = offset % _screen.width;
  int y = offset / _screen.width;
  int w = len / 4;
  int h = 1;
  _draw_rect(buf, x, y, w, h);
}

void init_device() {
  _ioe_init();
  //sprintf(char *dst， const char *format_string， ...)
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", _screen.width, _screen.height);
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
