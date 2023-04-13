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
  printf("-------\n");
  // unnecessary to update CF and OF in NEMU
  printf("id_src %08x\n", id_src->val);
  printf("id_dest %08x\n", id_dest->val);
  if(decoding.is_operand_size_16) {
    
    rtl_sext(&t2, &id_dest->val, 2);
    
    printf("id_aftersext %08x\n", t2);
    rtl_sar(&t0, &t2, &id_src->val);
    printf("id_dest_after %08x\n", t0);
    printf("-------\n");
    operand_write(id_dest, &t0);
    rtl_update_ZFSF(&t0, id_dest->width);
  }
  else {
    rtl_sar(&t0, &id_dest->val, &id_src->val);
    printf("id_dest %08x\n", t0);
    operand_write(id_dest, &t0);
    rtl_update_ZFSF(&t0, id_dest->width);
    
  }eflags_ignore = true;
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
  rtl_not(&id_dest->val);
  operand_write(id_dest, &id_dest->val);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  rtl_li(&t0, 0);
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
  print_asm_template1(not);
}
