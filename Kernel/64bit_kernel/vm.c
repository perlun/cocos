/* 
 * $Id$
 *
 * vm.c - Virtual Memory routines.
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008 Per Lundberg
 */

#include "io.h"
#include "vm.h"

/*static*/ void vm_init_physical_zone(uint64_t upper_memory_limit)
{
    // Physical memory VM zone initialization.
    //
    // Because we want to trap NULL pointer references, we MUST make sure to not map page 0 (0-4 KiB). Now, just ignoring it
    // means we waste 4 KiB of precious RAM and even though RAM is *extremely* cheap nowadays... 4 KiB is always KiB and it
    // feels lame and wrong to just ignore it. What we do is that we map it "on top" of the rest of the physical memory, and
    // set the upper memory boundary that can be used to upper_memory_limit + SMALL_PAGE_SIZE.

    uint64_t current_page_size = VM_SMALL_PAGE_SIZE;
    int row = 0, column = 0;
    for (uint64_t index = VM_SMALL_PAGE_SIZE; 
         index < upper_memory_limit + VM_SMALL_PAGE_SIZE;
         index += current_page_size)
    {
        if (index % VM_LARGE_PAGE_SIZE == 0)
        {
            current_page_size = VM_LARGE_PAGE_SIZE;
        }

        io_move_cursor(row, column);
        io_print_formatted("Page: %X-%X", index, index + current_page_size - 1);

        row++;

        // Extremely evil and hardwired but hey, we're talking about debug code anyway...
        if (row == 25)
        {
            row = 2;
            column = column + 30;
            
            if (column > 80)
            {
                break;
            }
        }
    }
}

void vm_init(uint64_t upper_memory_limit)
{


    // Alright; new page tables have been set up. We can now set CR3 to point at the newly created PML4 structure.
}
