#include "nemu.h"
#include "memory.h"
#include "device/mmio.h"

#ifndef PGSIZE
#define PGSIZE 4096
#endif

#ifndef PGMASK
#define PGMASK          (PGSIZE - 1)    // Mask for bit ops
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

paddr_t page_translate(vaddr_t vaddr){
  return vaddr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if(cpu.PG) {
    if ((addr & PGMASK) + len > PGSIZE) {
      /* this is a special case, you can handle it later.*/
      printf("vaddr_read : this is a special case, you can handle it later");
      assert(0);
    }
    else {
      paddr_t paddr = page_translate(addr);
      return paddr_read(paddr, len);
    }
  }
  else {
    return paddr_read(addr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if(cpu.PG) {
    if ((addr & PGMASK) + len > PGSIZE) {
      /* this is a special case, you can handle it later.*/
      printf("vaddr_write : this is a special case, you can handle it later");
      assert(0);
    }
    else {
      paddr_t paddr = page_translate(addr);
      return paddr_write(paddr, len, data);
    }
  }
  else{
    return paddr_write(addr, len, data);
  }
}
