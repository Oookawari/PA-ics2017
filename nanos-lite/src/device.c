/*
#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  char* buffer = (char* )buf;
  for(int i = 0; i < len; i++) {
    buffer[i] = dispinfo[offset + i];
  }

  memcpy(buf, dispinfo + offset, len);

}
void fb_write(const void *buf, off_t offset, size_t len) {
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
*/
#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
    int key_value = _read_key();
    if(key_value == _KEY_NONE){
        snprintf(buf, len, "t %d\n", _uptime());
    }
    else if(key_value & 0x8000){
        key_value ^= 0x8000;
        snprintf(buf, len, "kd %s\n", keyname[key_value]);
    }
    else{
        snprintf(buf, len, "ku %s\n", keyname[key_value]);
    }
    return strlen(buf);
    
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
    memcpy(buf, dispinfo + offset, len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
    int y, x;
    offset /= 4;
    x = offset % _screen.width;
    y = offset / _screen.width;
    _draw_rect(buf, x, y, len/4, 1);

}

void init_device() {
    _ioe_init();

    // TODO: print the string to array `dispinfo` with the format
    // described in the Navy-apps convention
    sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", _screen.width, _screen.height);
    Log("dispinfo:\n%s", dispinfo);
}
