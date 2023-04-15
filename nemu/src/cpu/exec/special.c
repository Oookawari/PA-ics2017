#include "cpu/exec.h"
#include "monitor/monitor.h"

make_EHelper(nop) {
  print_asm("nop");
}

make_EHelper(inv) {
  /* invalid opcode */

  uint32_t temp[2];
  vaddr_t ori_eip = cpu.eip;
  *eip = ori_eip;
  temp[0] = instr_fetch(eip, 4);
  temp[1] = instr_fetch(eip, 4);

  uint8_t *p = (void *)temp;
  printf("invalid opcode(eip = 0x%08x): %02x %02x %02x %02x %02x %02x %02x %02x ...\n\n",
      ori_eip, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

  extern char logo [];
  printf("There are two cases which will trigger this unexpected exception:\n"
      "1. The instruction at eip = 0x%08x is not implemented.\n"
      "2. Something is implemented incorrectly.\n", ori_eip);
  printf("Find this eip(0x%08x) in the disassembling result to distinguish which case it is.\n\n", ori_eip);
  printf("\33[1;31mIf it is the first case, see\n%s\nfor more details.\n\nIf it is the second case, remember:\n"
      "* The machine is always right!\n"
      "* Every line of untested code is always wrong!\33[0m\n\n", logo);

  nemu_state = NEMU_END;

  print_asm("invalid opcode");
}

make_EHelper(nemu_trap) {
  print_asm("nemu trap (eax = %d)", cpu.eax);

  printf("\33[1;31mnemu: HIT %s TRAP\33[0m at eip = 0x%08x\n\n",
      (cpu.eax == 0 ? "GOOD" : "BAD"), cpu.eip);
  nemu_state = NEMU_END;

#ifdef DIFF_TEST
  extern void diff_test_skip_qemu();
  diff_test_skip_qemu();
#endif
}

make_EHelper(rol) {
  if(id_dest->width == 2) {assert(0);}
  uint32_t count = id_src->val;
  uint32_t temp = count;
  uint32_t rm = id_dest->val;
  while(temp != 0) {
    uint32_t highbit = rm & 0x80000000;
    if(highbit == 0) {
      rm *= 2;
      rm += 0;
      rtl_set_ZF(&tzero);
    }
    else {
      rm *= 2;
      rm += 1;
      rtl_li(&t0, 1);
      rtl_set_ZF(&t0);
    }
    temp --;
  }
  rtl_li(&t3, rm);
  operand_write(id_dest, &t3);
  if(count == 1) {
    uint32_t highbit = rm & 0x80000000;
    if(highbit == 0) {
      if(cpu.CF == 0) {
        rtl_set_OF(&tzero);
      }
      else {
        rtl_li(&t0, 1);
        rtl_set_OF(&t0);
      }
    }
    else {
      if(cpu.CF == 0) {
        rtl_li(&t0, 1);
        rtl_set_OF(&t0);
      }
      else {
        rtl_set_OF(&tzero);
      }
    }
  }
  print_asm("rol");
}
