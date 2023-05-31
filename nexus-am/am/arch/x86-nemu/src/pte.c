#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);
void* memcpy(void*,const void*, int);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
  // 获取页目录基地址
  PDE *pgdir = (PDE *)p->ptr;

  // 获取虚拟地址的页目录索引、页表索引和页内偏移
  uint32_t dir_index = ((uint32_t)va >> 22) & 0x3FF;
  uint32_t table_index = ((uint32_t)va >> 12) & 0x3FF;
  //uint32_t offset = (uint32_t)va & 0xFFF;

  // 检查页目录项是否存在，若不存在则申请新的页表
  if (!(pgdir[dir_index] & 0x00000001)) {
    // 申请一页空闲的物理页
    PDE *new_pt = palloc_f();

    // 将新的页表物理地址填入页目录项
    pgdir[dir_index] = (uint32_t)new_pt & 0xFFFFF000;
    pgdir[dir_index] |= 0x00000001;
  }

  // 获取页表基地址
  PTE *pgtable = (PTE *)(pgdir[dir_index] & 0xFFFFF000);

  // 填写页表项，映射虚拟地址到物理地址
  pgtable[table_index] = ((uint32_t)pa & 0xFFFFF000) | 0x00000001;
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
  //ustack.end -= 4;
  *(uint32_t*)(ustack.end - 4) = 0; //参数，忽略为0
  //ustack.end -= 4;
  *(uint32_t*)(ustack.end - 8) = 0; //参数，忽略为0
  //ustack.end -= 4;
  *(uint32_t*)(ustack.end - 12) = 0; //参数，忽略为0
  //ustack.end -= 4;
  //*(uint32_t*)(ustack.end - 16) = cpu.eip; //eip
  *(uint32_t*)(ustack.end - 16) = 0; //在这里获取不到cpu
  _RegSet fake_trap;
  fake_trap.cs = 0x8;
  fake_trap.eip = (uintptr_t)entry;
  memcpy((uint32_t*)(ustack.end - sizeof(_RegSet)), &fake_trap, sizeof(_RegSet));
  return (_RegSet *)ustack.end - sizeof(_RegSet);
}
