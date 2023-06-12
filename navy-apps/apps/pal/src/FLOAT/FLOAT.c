#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>
struct float_
{
  uint32_t frac : 23;
  uint32_t exp : 8;
  uint32_t sign : 1;
};
FLOAT F_mul_F(FLOAT a, FLOAT b) {
  FLOAT res = (int64_t)a * (int64_t)b / 0xFFFF;
  return res;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  assert(b != 0);
  
  FLOAT res = a / b * 0xFFFF;
  return res;/*
  FLOAT x = Fabs(a);
  FLOAT y = Fabs(b);
  FLOAT z = x / y;
  x = x % y;

  for (int i = 0; i < 16; i++)
  {
    x <<= 1;
    z <<= 1;
    if (x >= y)
    {
      x -= y;
      z++;
    }
  }
  if (((a ^ b) & 0x80000000) == 0x80000000)
  {
    z = -z;
  }
  return z;*/
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */
  
  unsigned int* temp = (unsigned int *)&a;
  unsigned int S = (*temp) & 0x80000000;
  unsigned int E = (*temp) & 0x7F800000;
  unsigned int M = (*temp) & 0x007FFFFF;
  if(E == 255) {
    return 0x00000000;//这是NaN，或者正无穷，或者负无穷。不知道该怎么表示
  }
  else if(E == 0) {
    return 0x00000000;//这就是0
  }
  else{
    //阶码部分（E）：根据不同的精度E的位数不同（参照下图float与double的区别），表示小数点向右移动的位数。E>0 表示向右移动，E<0表示向左移动。
    //先给M加上省去的1
    M |= 0x00800000;
    E = E - 127;
    if(E > 0) {
      //向右移动3+4*5位，得到原本的表示，再向左移动E+16位，得到我们的表示，也就是说，需要向左移动E-7位
      //相当于左移（E-7）位
      if(E >= 7) {
        FLOAT res = M << (E - 7);
        return S ? -res : res;
      }
      else {
        FLOAT res = (M >> 7) << E;
        return S ? -res : res;
      }
    }
    else {
      FLOAT res = (M >> 7) >> (-E);
      return S ? -res : res;
    }
  }
}

FLOAT Fabs(FLOAT a) {
  unsigned int s = a & 0x80000000;
	if (s != 0) { return 0 - a; }
	else return a;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}
