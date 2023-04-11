#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //rtl_push(&((*id_dest).val));
  if(id_dest->type == OP_TYPE_MEM) {
    rtl_push(&id_dest->addr);
  }
  else if(id_dest->type == OP_TYPE_REG){
    rtl_push(&id_dest->val);
  }
  else if(id_dest->type == OP_TYPE_IMM){
    rtl_li(&t0, id_dest->imm);
    rtl_push(&t0);
  }
  print_asm_template1(push);
}

make_EHelper(pop) {
  //rtl_pop(&((*id_dest).val));
  rtl_pop(&t2);
  operand_write(id_dest, &t2);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  if (decoding.is_operand_size_16) {
    rtl_mv(&cpu.esp, &cpu.ebp);
    rtl_pop(&cpu.ebp);
    /*未完全实现*/
  }
  else{
    rtl_mv(&cpu.esp, &cpu.ebp);
    rtl_pop(&cpu.ebp);
  }
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    if((cpu.eax & 0x8000) == 0x8000) {
      cpu.gpr[2]._16 = 0xFFFF;
    } else {
      cpu.gpr[2]._16 = 0x0;
    }
  }
  else {
    if((cpu.eax & 0x80000000) == 0x80000000) {
      cpu.edx = 0xFFFFFFFF;
    } else {
      cpu.edx = 0x0;
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    if((cpu.eax & 0x8000) == 0x8000) {
      cpu.gpr[2]._16 = 0xFFFF;
    } else {
      cpu.gpr[2]._16 = 0x0;
    }
  }
  else {
    if((cpu.eax & 0x80000000) == 0x80000000) {
      cpu.edx = 0xFFFFFFFF;
    } else {
      cpu.edx = 0x0;
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}

make_EHelper(xchg) {
  id_src->type = OP_TYPE_REG;
  if (decoding.is_operand_size_16) {
    id_src->reg = R_AX;
    rtl_mv(&t2, &id_dest->val);
    rtl_li(&t1, reg_w(0));
    operand_write(id_dest, &t1);
    reg_w(0) = t2;
  }
  else {
    id_src->reg = R_EAX;
    rtl_mv(&t2, &id_dest->val);
    rtl_li(&t1, reg_l(0));
    operand_write(id_dest, &t1);
    reg_l(0) = t2;
  }
  print_asm_template2(xchg);
}
