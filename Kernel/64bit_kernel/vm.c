/* 
 * $Id$
 *
 * vm.c - Virtual Memory routines.
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008-2009 Per Lundberg
 */

#include "io.h"
#include "vm.h"

/*static*/ void vm_init_physical_zone(uint64_t upper_memory_limit)
{
    // Physical memory VM zone initialization.
    //
    // Because we want to trap NULL pointer references, we MUST make sure to not map page 0 (0-4 KiB). I had initially
    // thought about making this page mapped "above" all the rest of the RAM, but really, I think it will be hard to make
    // that work in a good way... If you have 4 GiB of RAM in the machine, mapping one extra page from 4 GiB to 4 GiB + 4 KiB
    // will require at least one extra page table (in this specific case, it will also require an extra page directory...).
    //
    // However, what we could do is to try and map this page in some of the "memory" holes in the low 2 meg area. Still, it
    // will be a bit bad because we cannot do normal 1-to-1-translations of addresses in this area if we do it like that.  We
    // try to find a suitable one-page memory need as we go along with the development instead, and use this memory for that
    // if we find it.

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
