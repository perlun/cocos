/* 
 * $Id$
 *
 * vmtest.c - Virtual Memory test program. This program is used to analyze how well the physical memory mapper works. I
 * started writing this as a program that wrote text to the kernel console, but it is much easier (and completely feasible)
 * to run this under GNU/Linux instead...
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008 Per Lundberg
 */

#include "stdio.h"
#include "vm.h"

static void vm_init_physical_zone(uint64_t upper_memory_limit)
{
    // Physical memory VM zone initialization.
    //
    // Because we want to trap NULL pointer references, we MUST make sure to not map page 0 (0-4 KiB). Now, just ignoring it
    // means we waste 4 KiB of precious RAM and even though RAM is *extremely* cheap nowadays... 4 KiB is always KiB and it
    // feels lame and wrong to just ignore it. What we do is that we map it "on top" of the rest of the physical memory, and
    // set the upper memory boundary that can be used to upper_memory_limit + SMALL_PAGE_SIZE.

    long current_page_size = VM_SMALL_PAGE_SIZE;
    long num_pages = 0;

    // We need at least one of each one of those.
    const long pml4_pages_needed = 1; // there is always just one PML4 structure.
    long pdp_pages_needed = 1;
    long pd_pages_needed = 1;
    long pt_pages_needed = 1;
    int pd_index = 1; // The first PD entry is used for the "small pages" in the beginning of RAM (0-2 MiB).
    int pt_index = 0;
    for (long index = VM_SMALL_PAGE_SIZE; 
         index < upper_memory_limit + VM_SMALL_PAGE_SIZE;
         index += current_page_size)
    {
        if (index % VM_LARGE_PAGE_SIZE == 0)
        {
            current_page_size = VM_LARGE_PAGE_SIZE;
            pt_index = 0;
        }

        printf("Page: %lX-%lX (page size %lX, page table %ld, page table index %d)\n", index, 
               index + current_page_size - 1, current_page_size, pt_pages_needed - 1,
               pt_index);
        num_pages++;

        if (current_page_size == VM_LARGE_PAGE_SIZE)
        {
            pd_index++;

            if (pd_index == 512)
            {
                pd_pages_nee
            }
        }
        else // current_page_size == VM_SMALL_PAGE_SIZE
        {
            pt_index++;
            
            if (pt_index == 512)
            {
                pt_pages_needed++;
                pt_index = 0;
            }
        }
    }

    long index = 0;
    current_page_size = VM_SMALL_PAGE_SIZE;
    printf("Special mapping for page: %lX-%lX (page size %lX)\n", index, index + current_page_size - 1, current_page_size);
    num_pages++;

    printf("Total number of pages being mapped: %lu\n", num_pages);
    printf("Number of paging-related 4 KiB pages needed: %lu\n", pml4_pages_needed + pdp_pages_needed + pd_pages_needed + 
           pt_pages_needed);
}

int main(void)
{
    vm_init_physical_zone(256 * 1024 * 1024);
}
