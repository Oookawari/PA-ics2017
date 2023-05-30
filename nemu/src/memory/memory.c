#include "nemu.h"
#include "memory.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#ifndef PGSIZE
#define PGSIZE 4096
#endif

#ifndef PGMASK
#define PGMASK (PGSIZE - 1)    // Mask for bit ops

#endif

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1) {
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
  else {
    return mmio_read(addr, len, map_NO);
  }
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1) {
    memcpy(guest_to_host(addr), &data, len);
  }
  else {
    mmio_write(addr, len, data, map_NO);
  }
  //memcpy(guest_to_host(addr), &data, len);
}

paddr_t page_translate(vaddr_t vaddr, bool is_write){
  // 获取 CR3 寄存器中的页目录表基址
  paddr_t directory_base = cpu.cr3.page_directory_base;

  // 计算页目录项地址
  uint32_t directory_index = (vaddr >> 22) & 0x3FF;
  paddr_t directory_entry_addr = (directory_base & 0xFFFFF000) | (directory_index << 2);

  // 读取页目录项并进行有效性检查
  PDE directory_entry;
  directory_entry.val = paddr_read(directory_entry_addr, 4);
  Assert(directory_entry.present, "directory_entry.present为0");

  // 计算页表项地址
  uint32_t table_index = (vaddr >> 12) & 0x3FF;
  paddr_t table_entry_addr = (directory_entry.page_frame << 12) | (table_index << 2);

  // 读取页表项并进行有效性检查
  PTE table_entry;
  table_entry.val = paddr_read(table_entry_addr, 4);
  Assert(table_entry.present, "table_entry.present为0");

  // 检查 PDE 的访问位并更新
  if (!directory_entry.accessed) {
    directory_entry.accessed = 1;
    paddr_write(directory_entry_addr, 4, directory_entry.val);
  }

  // 检查 PTE 的访问位和脏位，并更新
  if (!table_entry.accessed || (!table_entry.dirty && is_write)) {
    table_entry.accessed = 1;
    table_entry.dirty = is_write;
    paddr_write(table_entry_addr, 4, table_entry.val);
  }

  // 计算物理地址并返回
  paddr_t paddr = (table_entry.page_frame << 12) | (vaddr & 0xFFF);
  return paddr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if(cpu.cr0.paging) {
    if ((addr & PGMASK) + len > PGSIZE) {
      /* this is a special case, you can handle it later.*/
      printf("vaddr_read : this is a special case, you can handle it later");
      assert(0);
    }
    else {
      paddr_t paddr = page_translate(addr, false);
      return paddr_read(paddr, len);
    }
  }
  else {
    return paddr_read(addr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if(cpu.cr0.paging) {
    if ((addr & PGMASK) + len > PGSIZE) {
      /* this is a special case, you can handle it later.*/
      printf("vaddr_write : this is a special case, you can handle it later");
      assert(0);
    }
    else {
      paddr_t paddr = page_translate(addr, true);
      return paddr_write(paddr, len, data);
    }
  }
  else{
    return paddr_write(addr, len, data);
  }
}
