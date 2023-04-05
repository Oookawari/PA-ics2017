#include "cpu/exec.h"
bool eflags_ignore;
make_EHelper(test) {
  rtl_and(&t2, &id_dest->val, &id_src->val);
  //operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width); 
  rtl_li(&t0, 0);
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  rtl_li(&t0, 0);
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  rtl_li(&t0, 0);
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  rtl_li(&t0, 0);
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  eflags_ignore = true;
  print_asm_template2(sar);
}

make_EHelper(shl) {
  // unnecessary to update CF and OF in NEMU
  //rtl_shl(&t0, &id_dest->val, &id_src->val);
  rtl_shl(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  //printf("shl %08x\n", t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  eflags_ignore = true;
  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  eflags_ignore = true;
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
