#ifndef PAGING_H
#define PAGING_H

#include "types.h"

// Paging Initialization functions
void paging_init();
void paging_enable(uint32_t* pdir_addr);
void paging_switch(uint32_t mb_va, uint32_t mb_pa);

#endif /* PAGING_H */


