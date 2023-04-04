#ifndef __RTL_H__
#define __RTL_H__

#include "nemu.h"

extern rtlreg_t t0, t1, t2, t3;
extern const rtlreg_t tzero;
/* RTL basic instructions */

static inline void rtl_li(rtlreg_t* dest, uint32_t imm) {
  *dest = imm;
}

#define c_add(a, b) ((a) + (b))
#define c_sub(a, b) ((a) - (b))
#define c_and(a, b) ((a) & (b))
#define c_or(a, b)  ((a) | (b))
#define c_xor(a, b) ((a) ^ (b))
#define c_shl(a, b) ((a) << (b))
#define c_shr(a, b) ((a) >> (b))
#define c_sar(a, b) ((int32_t)(a) >> (b))
#define c_slt(a, b) ((int32_t)(a) < (int32_t)(b))
#define c_sltu(a, b) ((a) < (b))

#define make_rtl_arith_logic(name) \
  static inline void concat(rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  } \
  static inline void concat3(rtl_, name, i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
    *dest = concat(c_, name) (*src1, imm); \
  }


make_rtl_arith_logic(add)
make_rtl_arith_logic(sub)
make_rtl_arith_logic(and)
make_rtl_arith_logic(or)
make_rtl_arith_logic(xor)
make_rtl_arith_logic(shl)
make_rtl_arith_logic(shr)
make_rtl_arith_logic(sar)
make_rtl_arith_logic(slt)
make_rtl_arith_logic(sltu)

static inline void rtl_mul(rtlreg_t* dest_hi, rtlreg_t* dest_lo, const rtlreg_t* src1, const rtlreg_t* src2) {
  asm volatile("mul %3" : "=d"(*dest_hi), "=a"(*dest_lo) : "a"(*src1), "r"(*src2));
}

static inline void rtl_imul(rtlreg_t* dest_hi, rtlreg_t* dest_lo, const rtlreg_t* src1, const rtlreg_t* src2) {
  asm volatile("imul %3" : "=d"(*dest_hi), "=a"(*dest_lo) : "a"(*src1), "r"(*src2));
}

static inline void rtl_div(rtlreg_t* q, rtlreg_t* r, const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  asm volatile("div %4" : "=a"(*q), "=d"(*r) : "d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}

static inline void rtl_idiv(rtlreg_t* q, rtlreg_t* r, const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  asm volatile("idiv %4" : "=a"(*q), "=d"(*r) : "d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}

static inline void rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len) {
  *dest = vaddr_read(*addr, len);
}

static inline void rtl_sm(rtlreg_t* addr, int len, const rtlreg_t* src1) {
  vaddr_write(*addr, len, *src1);
}

static inline void rtl_lr_b(rtlreg_t* dest, int r) {
  *dest = reg_b(r);
}

static inline void rtl_lr_w(rtlreg_t* dest, int r) {
  *dest = reg_w(r);
}

static inline void rtl_lr_l(rtlreg_t* dest, int r) {
  *dest = reg_l(r);
}

static inline void rtl_sr_b(int r, const rtlreg_t* src1) {
  reg_b(r) = *src1;
}

static inline void rtl_sr_w(int r, const rtlreg_t* src1) {
  reg_w(r) = *src1;
}

static inline void rtl_sr_l(int r, const rtlreg_t* src1) {
  reg_l(r) = *src1;
}

/* RTL psuedo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_lr_l(dest, r); return;
    case 1: rtl_lr_b(dest, r); return;
    case 2: rtl_lr_w(dest, r); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, int width, const rtlreg_t* src1) {
  switch (width) {
    case 4: rtl_sr_l(r, src1); return;
    case 1: rtl_sr_b(r, src1); return;
    case 2: rtl_sr_w(r, src1); return;
    default: assert(0);
  }
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    if(*src) (&cpu)->f = 1; \
    else (&cpu)->f = 0;\
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    if((&cpu)->f) rtl_li(dest, 1); \
    else rtl_li(dest, 0);\
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_mv(rtlreg_t* dest, const rtlreg_t *src1) {
  // dest <- src1
  *dest = *src1;
}

static inline void rtl_not(rtlreg_t* dest) {
  // dest <- ~dest
  *dest = (~(*dest));
}

static inline void rtl_sext(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  assert(width == 1 || width == 2 || width == 4);
  if(width == 1){
    *dest = *src1 & 0x0000007F;
    (*src1 & 0x00000080) ? (*dest = *dest | (0xFFFFFF80)) : (*dest = *dest | (0x00000000)); 
  } else if(width == 2) {
    
    *dest = *src1 & 0x00007F00;
    (*src1 & 0x00008000) ? (*dest = *dest | (0xFFFF8000)) : (*dest = *dest | (0x00000000)); 
  } else if(width == 4) {

    *dest = *src1 & 0x7F000000;
    (*src1 & 0x80000000) ? (*dest = *dest | (0x80000000)) : (*dest = *dest | (0x00000000)); 
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  reg_l(R_ESP) = reg_l(R_ESP) - 4;
  // esp <- esp - 4
  rtl_sm(&cpu.esp, 4, src1);
  // M[esp] <- src1
  //TODO();
}

static inline void rtl_pop(rtlreg_t* dest) {
  rtl_lm(dest, &cpu.esp, 4);
  // dest <- M[esp]
  reg_l(R_ESP) = reg_l(R_ESP) + 4;
  // esp <- esp + 4
}

static inline void rtl_eq0(rtlreg_t* dest, const rtlreg_t* src1) {
  // dest <- (src1 == 0 ? 1 : 0)
  *dest = ((*src1) ? 0 : 1);
}

static inline void rtl_eqi(rtlreg_t* dest, const rtlreg_t* src1, int imm) {
  // dest <- (src1 == imm ? 1 : 0)
  *dest = ((*src1 == imm) ? 1 : 0);
}

static inline void rtl_neq0(rtlreg_t* dest, const rtlreg_t* src1) {
  // dest <- (src1 != 0 ? 1 : 0)
  *dest = ((*src1) ? 1 : 0);
  TODO();
}

static inline void rtl_msb(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  assert(width == 1 || width == 2 || width == 4);
  bool has_sign = false;
  if(width == 1){
    has_sign = (*src1 & 0x00000080);
  } else if(width == 2) {
    has_sign = (*src1 & 0x00008000);
  } else if(width == 4) {
    has_sign = (*src1 & 0x80000000);
  }
  if(has_sign)
    rtl_li(dest, 1);
  else
    rtl_li(dest, 0);
}

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  assert(width == 1 || width == 2 || width == 4);
  if(width == 1){
    bool not_zero = (*result & 0x000000FF);
    if(not_zero) {
      rtl_li(&t1, 0);
      rtl_set_ZF(&t1);
    }
    else{
      rtl_li(&t1, 1);
      rtl_set_ZF(&t1);
    }
  } else if(width == 2) {
    bool not_zero = (*result & 0x0000FFFF);
    if(not_zero) {
      rtl_li(&t1, 0);
      rtl_set_ZF(&t1);
    }
    else{
      rtl_li(&t1, 1);
      rtl_set_ZF(&t1);
    }
  } else if(width == 4) {
    printf("result width = 4, %08x\n", *result);
    bool not_zero = (*result & 0xFFFFFFFF);
    printf("not_zero, %d\n", not_zero);
    if(not_zero) {
      printf("ZF_UNSET!!!!! width = 4\n");
      rtl_li(&t1, 0);
      rtl_set_ZF(&t1);
    }
    else{
      rtl_li(&t1, 1);
      rtl_set_ZF(&t1);
    }
  }
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  assert(width == 1 || width == 2 || width == 4);
  if(width == 1){
    bool has_sign = (*result & 0x00000080);
    if(has_sign) {
      rtl_li(&t1, 1);
      rtl_set_SF(&t1);
    }
    else{
      rtl_li(&t1, 0);
      rtl_set_SF(&t1);
    }
  } else if(width == 2) {
      
    bool has_sign = (*result & 0x00008000);
    if(has_sign) {
      rtl_li(&t1, 1);
      rtl_set_SF(&t1);
    }
    else{
      rtl_li(&t1, 0);
      rtl_set_SF(&t1);
    }
  } else if(width == 4) {
    unsigned int temp = (*result & 0x80000000);
    //bool has_sign = (*result & 0x80000000);
    bool has_sign = (temp == 0 ? 0 : 1);
    if(has_sign) {
      rtl_li(&t1, 1);
      rtl_set_SF(&t1);
    }
    else{
      rtl_li(&t1, 0);
      rtl_set_SF(&t1);
    }
  }
}

/*WARNING 调用该函数会覆盖t1！！！！！请注意*/
static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
