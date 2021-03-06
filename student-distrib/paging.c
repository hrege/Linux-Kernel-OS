// Paging Initialization and Enabling
//include defines etc.
#include "x86_desc.h"
#include "idt_init.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "paging.h"


static uint32_t page_directory[TABLE_SIZE] __attribute__((aligned (4096)));   // Construct a page directory
static uint32_t page_table[TABLE_SIZE] __attribute__((aligned (4096)));       // Construct a page table
static uint32_t fish_page_table[TABLE_SIZE] __attribute__((aligned (4096)));    //Construct a user-accessible page table
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
    //Set PDE for the Page Table for 0MB-4MB in Physical Memory
    //page_directory[0] = ((((uint32_t)&page_table) & 0xFFFFF000) | 0x007);

    //Set PDE for 4MB kernel page for 4MB-8MB in Physical Memory
    page_directory[1] = 0x00400083;

    //Set rest of PDEs to "not present"
    int i;
    for(i = 2; i < TABLE_SIZE; i++){
        page_directory[i] = 0x00000000 | READ_WRITE;
    }
        

    //Set rest of PTEs to "not present"
    int j;
    for(j = 0; j < TABLE_SIZE; j++){
        page_table[j] = (j * PTE_MOVE) | READ_WRITE;
    }

    //Set PTE for the video memory

    page_table[VIDEO_IDX] = VIDEO_LOC | PTE_ENTRY_VAL;
    page_table[VIDEO_TERM_0] = VIDEO_LOC | PTE_ENTRY_VAL;
    page_table[VIDEO_TERM_1] = VIDEO_LOC_1 | PTE_ENTRY_VAL;
    page_table[VIDEO_TERM_2] = VIDEO_LOC_2 | PTE_ENTRY_VAL;

    page_directory[0] = ((uint32_t)page_table) | PDE_ENTRY_VAL;

    page_directory[2] = 0x00000000 | READ_WRITE;
    page_directory[3] = 0x00000000 | READ_WRITE;

    page_directory[VID_MEM_MB_LOC] = ((uint32_t)fish_page_table) | PTE_ENTRY_VAL;


    //Set PDE for 0MB-4MB in Physical Memory (mapped to 132MB Virtual)
    fish_page_table[VIDEO_IDX] = VIDEO_LOC | PTE_ENTRY_VAL;
    fish_page_table[VIDEO_TERM_0] = VIDEO_LOC | PTE_ENTRY_VAL;
    fish_page_table[VIDEO_TERM_1] = VIDEO_LOC_1 | PTE_ENTRY_VAL;
    fish_page_table[VIDEO_TERM_2] = VIDEO_LOC_2 | PTE_ENTRY_VAL;


    asm volatile ("movl %%cr3, %%eax  \n\
                   movl %%eax, %%cr3      \n\
                   "
                   :
                   :
                   : "eax", "memory", "cc"
    );

    // Set control registers to enable paging.
    paging_enable(page_directory);
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
void paging_enable(uint32_t* pdir_addr){
    asm volatile ("movl %0, %%eax      \n\
            movl %%eax, %%cr3          \n\
            movl %%cr4, %%eax          \n\
            orl  $0x00000010, %%eax     \n\
            movl %%eax, %%cr4          \n\
            movl %%cr0, %%eax           \n\
            orl  $0x80000001, %%eax     \n\
            movl %%eax, %%cr0          \n\
            "
            :
            : "m"(pdir_addr)
            : "eax", "memory", "cc"
    );
    return;
}

/* Author: Austin
 * void paging_switch(uint32_t mb_va, uin32_t mb_pa)
 *      Inputs: mb_va - MB location of virtual address
 *              mb_pa - MB location of physical address
 *      Return Value: void
 *      Function: Sets 4MB-page PDE at the virtual address to
 *                the proper physical address
 *      Side effects: Flushes the entire TLB
 */
void paging_switch(uint32_t mb_va, uint32_t mb_pa){
    uint32_t phys_addr = mb_pa/PAGE_MB_NUM;
    uint32_t vir_addr = mb_va/PAGE_MB_NUM;
    page_directory[vir_addr] = (0x00000087 | (phys_addr << PDE_OFFSET));
        asm volatile ("movl %%cr3, %%eax  \n\
                   movl %%eax, %%cr3      \n\
                   "
                   :
                   :
                   : "eax", "memory", "cc"
    );
}

/* void terminal_activate(int term_num)
 *      Author: Hershel/Sam
 *      Inputs: term_num - number of terminal to "activate"
 *      Return Value: void
 *      Function: Copies the correct recorded characters to the video memory,
 *                flushes the TLB, and updates the cursor
 *      Side effects: Flushes the entire TLB
 */
void terminal_activate(int term_num){
    switch(term_num){
        case 0:
        memcpy((char*)VIDEO_LOC, (char*)VIDEO_LOC_0, SCREEN_SIZE); 
        page_table[VIDEO_TERM_0] = VIDEO_LOC | PTE_ENTRY_VAL;
        fish_page_table[VIDEO_TERM_0] = VIDEO_LOC | PTE_ENTRY_VAL;
        break;

        case 1:
        memcpy((char*)VIDEO_LOC, (char*)VIDEO_LOC_1, SCREEN_SIZE);
        page_table[VIDEO_TERM_1] = VIDEO_LOC | PTE_ENTRY_VAL;
        fish_page_table[VIDEO_TERM_1] = VIDEO_LOC | PTE_ENTRY_VAL;
        break;

        case 2:
        memcpy((char*)VIDEO_LOC, (char*)VIDEO_LOC_2, SCREEN_SIZE);
        page_table[VIDEO_TERM_2] = VIDEO_LOC | PTE_ENTRY_VAL;
        fish_page_table[VIDEO_TERM_2] = VIDEO_LOC | PTE_ENTRY_VAL;
        break;
    }
    asm volatile ("movl %%cr3, %%eax  \n\
           movl %%eax, %%cr3      \n\
           "
           :
           :
           : "eax", "memory", "cc"
    );

    update_cursor(screen_x[term_num], screen_y[term_num]);
}

/* void terminal_deactivate(int term_num)
 *      Author: Hershel/Sam
 *      Inputs: term_num - number of terminal to "deactivate"
 *      Return Value: void
 *      Function: Copies the visible terminal's characters into its own physical memory location,
 *                flushes the TLB, and updates the cursor
 *      Side effects: Flushes the entire TLB
 */
void terminal_deactivate(int term_num){
    switch(term_num){
        case 0:
        page_table[VIDEO_TERM_0] = VIDEO_LOC_0 | PTE_ENTRY_VAL;
        fish_page_table[VIDEO_TERM_0] = VIDEO_LOC_0 | PTE_ENTRY_VAL;
        asm volatile ("movl %%cr3, %%eax  \n\
               movl %%eax, %%cr3      \n\
               "
               :
               :
               : "eax", "memory", "cc"
        );
        memcpy((char*)VIDEO_LOC_0, (char*)VIDEO_LOC, SCREEN_SIZE);
        break;

        case 1:
        page_table[VIDEO_TERM_1] = VIDEO_LOC_1 | PTE_ENTRY_VAL;
        fish_page_table[VIDEO_TERM_1] = VIDEO_LOC_1 | PTE_ENTRY_VAL;
        asm volatile ("movl %%cr3, %%eax  \n\
               movl %%eax, %%cr3      \n\
               "
               :
               :
               : "eax", "memory", "cc"
        );
        memcpy((char*)VIDEO_LOC_1, (char*)VIDEO_LOC, SCREEN_SIZE);
        break;

        case 2:
        page_table[VIDEO_TERM_2] = VIDEO_LOC_2 | PTE_ENTRY_VAL;
        fish_page_table[VIDEO_TERM_2] = VIDEO_LOC_2 | PTE_ENTRY_VAL;
        asm volatile ("movl %%cr3, %%eax  \n\
               movl %%eax, %%cr3      \n\
               "
               :
               :
               : "eax", "memory", "cc"
        );
        memcpy((char*)VIDEO_LOC_2, (char*)VIDEO_LOC, SCREEN_SIZE);
        break;
    }
}
