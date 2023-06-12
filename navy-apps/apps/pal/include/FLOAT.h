#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "assert.h"

typedef int FLOAT;

static inline float F2f(FLOAT a) {
  float res = (float)a / 0xFFFF;
  return res;
}


static inline int F2int(FLOAT a) {
  int res = a >> 16;
  return res;
}

static inline FLOAT int2F(int a) {
  assert(a <= 0x7FFF && a >= -0x7FFF);
  FLOAT res = a << 16;
  return res;
}

static inline FLOAT F_mul_int(FLOAT a, int b) {
  FLOAT res = a * b;
  return res;
}

static inline FLOAT F_div_int(FLOAT a, int b) {
  FLOAT res = a / b;
  return res;
}

FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT Fsqrt(FLOAT);
FLOAT Fpow(FLOAT, FLOAT);

#endif
