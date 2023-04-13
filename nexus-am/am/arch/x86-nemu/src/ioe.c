#include <am.h>
#include <x86.h>

#define I8042_DATA_PORT 0x60
#define I8042_STATUS_PORT 0x64
#define RTC_PORT 0x48   // Note that this is not standard
//RTC(Real Time Clock),初始化时将会注册 0x48 处的端口作为 RTC 寄存器,CPU 可以通
//过 I/O 指令访问这一寄存器,获得当前时间(单位是 ms)
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  unsigned long now_time = inl(RTC_PORT);

  return now_time - boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  //for (i = 0; i < _screen.width * _screen.height; i++) {
   // fb[i] = i;
  //}
  for(int c = x; c < x + w; c++) {
    for(int b = y; b < y + h; b++) {
      fb[c * _screen.width + b] = *pixels; 
    }
  }
}

void _draw_sync() {
}

int _read_key() {
  if(!inb(I8042_STATUS_PORT))
    return _KEY_NONE;
  else if(inl(I8042_STATUS_PORT)) {
    unsigned long key = inl(I8042_DATA_PORT);
    return key;
  }
  return _KEY_NONE;
}
