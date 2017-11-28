#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define VIDEO_LOC       0x000B8000

// Paging Initialization functions
void paging_init();
void paging_enable(uint32_t* pdir_addr);
extern void paging_switch(uint32_t mb_va, uint32_t mb_pa);

#endif /* PAGING_H */


