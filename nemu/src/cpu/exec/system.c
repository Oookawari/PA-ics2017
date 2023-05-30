#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  cpu.IDTR.limit = vaddr_read(id_dest->addr, 2);

  if (decoding.is_operand_size_16)
      cpu.IDTR.base = vaddr_read(id_dest->addr + 2, 3);
  else
      cpu.IDTR.base = vaddr_read(id_dest->addr + 2, 4);

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  if(id_dest->reg == 0) {
    rtl_mv(&cpu.cr0.val, &id_src->val);
  }
  else if(id_dest->reg == 3) {
    rtl_mv(&cpu.cr3.val, &id_src->val);
  }
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  if(id_src->reg == 0) {
    rtl_mv(&t0, &cpu.cr0.val);
  }
  else if(id_src->reg == 3) {
    rtl_mv(&t0, &cpu.cr3.val);
  }
  operand_write(id_dest, &t0);
  //rtl_sr_l(id_dest, &t0);
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  raise_intr(id_dest->val, decoding.seq_eip);

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  rtl_pop(&t0);
  decoding.jmp_eip = t0;
  decoding.is_jmp = true;
  rtl_pop(&t0);
  cpu.cs = t0;
  rtl_pop(&t0);
  cpu.eflags = t0;\
  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  t0 = pio_read(id_src->val, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  pio_write(id_dest->val, id_dest->width, id_src->val);
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
