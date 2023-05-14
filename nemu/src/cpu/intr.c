#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //依次将 EFLAGS,CS,EIP 寄存器的值压入堆栈
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
  //rtl_li(&t1, cpu.eip);
  rtl_li(&t1, ret_addr);
  rtl_push(&t1);
  //从 IDTR 中读出 IDT 的首地址
  uint32_t idtr_addr = cpu.IDTR.base;
  uint32_t idt_addr = idtr_addr + NO * 8;
  //根据异常(中断)号在 IDT 中进行索引,找到一个门描
  uint32_t low = vaddr_read(idt_addr, 4);
  uint32_t high = vaddr_read(idt_addr + 4, 4);
  //将门描述符中的 offset 域组合成目
  uint32_t target = (low & 0x0000FFFF) + (high & 0xFFFF0000);
  //cpu.eip = target;

  decoding.jmp_eip = target;
  decoding.is_jmp = true;
}

void dev_raise_intr() {
}
