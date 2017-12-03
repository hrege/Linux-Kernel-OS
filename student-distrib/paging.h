#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define VIDEO_IDX       0xB8                       //VIDEO from lib.c without 3 least significant bits
#define VIDEO_TERM_0       0xB9 
#define VIDEO_TERM_1       0xBA  
#define VIDEO_TERM_2       0xBB  
#define TABLE_SIZE      1024                     //1024 entries in Page directory and Page table
#define PAGE_MB_NUM     4
#define PTE_OFFSET      12
#define PDE_OFFSET      22
#define VIDEO_LOC       0x000B8000
#define VIDEO_LOC_0       0x000B9000
#define VIDEO_LOC_1       0x000BA000
#define VIDEO_LOC_2       0x000BB000
#define PTE_MOVE        0x1000
#define READ_WRITE      2
#define PTE_ENTRY_VAL   7
#define PDE_ENTRY_VAL   3
#define VID_MEM_MB_LOC  132/4
#define SCREEN_SIZE     4000  

// Paging Initialization functions
void paging_init();
void paging_enable(uint32_t* pdir_addr);
extern void paging_switch(uint32_t mb_va, uint32_t mb_pa);
extern void terminal_activate(int term_num);
extern void terminal_deactivate(int term_num);

#endif /* PAGING_H */


