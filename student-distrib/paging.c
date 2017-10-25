// Paging Initialization and Enabling
//////////////////
//include defines etc.
#include "x86_desc.h"
#include "idt_init.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "paging.h"

#define VIDEO 0xB8000                       //Identical definition as in lib.c

//extern tss_t tss;

#define TABLE_SIZE 1024                     //1024 entries in Page directory and Page table
#define VID_PTE VIDEO/TABLE_SIZE/4          //Video memory at 0xB8000 bytes addressable = index 184 in table

//Initialize Page Directory and Page Table
static uint32_t page_directory[TABLE_SIZE] __attribute__((aligned (4*TABLE_SIZE)));         // Construct a page directory
static uint32_t page_table[TABLE_SIZE] __attribute__((aligned (4*TABLE_SIZE)));             // Construct a page table     

/* Author: Austin
        PDE and PTE bits based on ISA Vol.3, pg. 3-32
            PDE for 4MB-Page
                [31:22] - Page Base Address
                [21:13] - Reserved (set to 0)
                [12] - Page Table Attribute Index
                [11:9] - Available for system use
                [8] - Global page (1 = global, 0 = not global)
                [7] - Page size (1 = 4MB-page, 0 = 4kB-page table)
                [6] - Dirty (1 = Written to, 0 = Clean or initialized)
                [5] - Accessed (1 = Accessed, 0 = Not accessed or initialized)
                [4] - Cache disabled (1 = Cache disabled, 0 = enabled)
                [3] - Write-through (1 = Enable, 0 = Disable)
                [2] - User/Supervisor (0 = Supervisor, 1 = User)
                [1] - Read/Write (1 = R/W, 0 = R)
                [0] - Present (1 = Present, 0 = Absent)
            PDE for 4kB-Page Table
                [31:12] - Page-Table Base Address
                [11:9] - Available for system use
                [8] - Global page (ignored)
                [7] - Page size
                [6] - Reserved (set to 0)
                [5] - Accessed
                [4] - Cache disabled
                [3] - Write-through
                [2] - User/Supervisor
                [1] - Read/Write
                [0] - Present
            PTE for 4kB-Page
                [31:12] - Page Base Address
                [11:9] - Available for system use
                [8] - Global page
                [7] - Page Table Attribute Index
                [6] - Dirty
                [5] - Accessed
                [4] - Cache disabled
                [3] - Write-through
                [2] - User/Supervisor
                [1] - Read/Write
                [0] - Present
    */


/* Author: Austin
 * void paging_init()
 *      Inputs: none
 *      Return Value: void
 *      Function: Initializes two arrays of size 1024: one page directory and one page table.
 *                Sets PDEs and PTEs to proper values, and calls paging_enable to set
 *                flags to proper values. 
 *      Side effects: Alters CR0, CR3 and CR4
 */

void paging_init(){         
    // Set control registers to enable paging.          
    paging_enable(page_directory);    
    
    //Set PDE for the Page Table for 0MB-4MB in Physical Memory
    page_directory[0] = (((int)page_table & 0xFFFFF000) | 0x01B);

    //Set PDE for 4MB kernel page for 4MB-8MB in Physical Memory
    page_directory[1] = 0x0400019B;

    //Set rest of PDEs to "not present"
    int i;
    for(i = 2; i < TABLE_SIZE; i++)
        page_directory[i] = 0x00000000;

    //Set rest of PTEs to "not present"
    int j;
    for(j = 0; j < TABLE_SIZE; j++)
        page_table[j] = 0x00000000;
        
    //Set PTE for the video memory
    page_table[VID_PTE] = 0x000B811B;
}



/* Author: Austin
 * void* paging_enable(uint32_t* pdir_addr)
 *      Inputs: pdir_addr - pointer to the page directory
 *      Return Value: void
 *      Function: Enables PE Flag (CR0[0]) for protection enabling.
 *                Enables PG Flag (CR0[31]) for paging.
 *                Enables PSE Flag (CR4[4]) for size extensions.
 *                Sets CR3 to pdir_addr value.
 *      Side effects: Alters CR0, CR3 and CR4
 */
//, uint32_t r1, unint32_t r2, unint32_t r3
void paging_enable(uint32_t* pdir_addr){
    asm volatile ("                   \n\
            movl %%cr0, %%eax            \n\
            orl  0x80000001, %%eax     \n\
            movl %%eax, %%cr0            \n\
            movl %%cr4, %%eax            \n\
            orl  0x00000010, %%eax     \n\
            movl %%eax, %%cr4           \n\
            movl %0, %%eax      \n\
            movl %%eax, %%cr3            \n\
            "
            :
            : "m"(pdir_addr)
            : "eax", "memory", "cc"
    );
    return;
}
/* Things to fix:
 * - Need the % for registers?
 * - Move jmp instruction (or take out)
 * - Add addresses for cr0, cr3, and cr4
 * - Video memory location?
 */


