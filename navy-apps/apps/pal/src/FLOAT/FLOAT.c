#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>
FLOAT F_mul_F(FLOAT a, FLOAT b) {
  return ((int64_t)a * (int64_t)b) >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  assert(b != 0);
  FLOAT x = Fabs(a);
  FLOAT y = Fabs(b);
  FLOAT temp1 = x / y;
  FLOAT res_h16 = temp1 << 16;
  FLOAT res_l16 = 0x00000000;
  x = x % y;
  uint32_t mask = 0x1;
  //移位算余数
  for (int i = 15; i >= 0; i--) {
    x <<= 1;
    if (x >= y) {
      x -= y;
      res_l16 |= (0x1 << i);
    }
  }
  FLOAT res = res_h16 | res_l16;
  uint32_t judge = ((a ^ b) & 0x80000000);
  if (judge == 0) {
    return res;
  }
  else return -res;
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
  //printf("原始数据：%08x\n", *temp);
  unsigned int S = (*temp) & 0x80000000;
  //printf("S: %08x\n", S);
  unsigned int E = (*temp) & 0x7F800000;
  E = E >> 23;
  //printf("E: %08x\n", E);
  unsigned int M = (*temp) & 0x007FFFFF;
  //printf("M: %08x\n", M);
  int exp;
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
    //printf("M 后: %08x\n", M);
    int Exp = E - 127;
    //printf("E 后: %08x\n", E);
    if(Exp > 0) {
      //向右移动3+4*5位，得到原本的表示，再向左移动E+16位，得到我们的表示，也就是说，需要向左移动E-7位
      //相当于左移（E-7）位
      if(Exp >= 7) {
        FLOAT res = M << (Exp - 7);
        
        //printf("我的结果: %08x\n",res);
        return S ? -res : res;
      }
      else {
        FLOAT res = (M >> 7) << Exp;
        
        //printf("我的结果: %08x\n",res);
        return S ? -res : res;
      }
    }
    else {
      FLOAT res = (M >> 7) >> (-Exp);
      return S ? -res : res;
    }
  }
}

FLOAT Fabs(FLOAT a) {
  unsigned int s = a & 0x80000000;
	if (s != 0) { return -a; }
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
