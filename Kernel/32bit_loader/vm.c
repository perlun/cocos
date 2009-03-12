/*
 * $Id$
 *
 * vm.c - Virtual Memory-related functions.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 * Copyright: (C) 2008-2009 Per Lundberg
 */

#include "common/misc.h"
#include "common/vm.h"
#include "io.h"
#include "memory.h"
#include "vm.h"

// Virtual memory is set up by this file.
//
// Why initialize virtual memory in the 32-bit loader, rather than in the 64-bit kernel? Well, good question. I don't
// remember the answer actually :-), it was a decision I took a couple of months ago. It might be this: we *have* to set up
// paging anyway, before we jump into 64-bit hyperspace. So, we might as well set up some paging structures that can actually
// be used by the 64-bit kernel, rather than setting up something temporary that will only be used until the 64-bit VM
// initilization takes place. It's not as if this 32-bit loader is designed to load anything else than *our* specific 64-bit
// kernel anyway... (and even in that case, it wouldn't be a problem since the 64-bit kernel can just overwrite the 32-bit
// stuff if it wants to)

// Define this to get some VM initialization debugging output. Good for checking out that the algorithms work properly.
#define VM_DEBUG 1

// Paging structures.
static pml4e_t *pml4 = (pml4e_t *)VM_STRUCTURES_PML4_ADDRESS;
static pdpe_t *pdp = (pdpe_t *)VM_STRUCTURES_PDP_ADDRESS;
static pde_t *pd;
static pte_t *low_pt = (pte_t *)VM_STRUCTURES_LOW_PAGE_TABLE_ADDRESS;

/**
 * Map a physical page into the virtual memory zone reserved for "physical" address space. (1-to-1 mapped)
 *
 * Ideally, this function should be supplied the address to the PML4 etc by a parameter or similar. Right now, we make it
 * pretty hardwired. If we supply the PML4, we still have to tell the function where all the other tables should be placed so
 * it's not so easy to make it generic like that...
 *
 * @param virtual_page  The number of the page that should be mapped (in the virtual address space).
 * @param physical_page  The number of the page that should be mapped (in the physical address space)
 * @param page_size  The size of the page that should be mapped.
 */
static void vm_map_physical_memory(uint64_t virtual_page, uint64_t physical_page, int page_size)
{
    if (page_size != VM_SMALL_PAGE_SIZE &&
        page_size != VM_LARGE_PAGE_SIZE)
    {
        io_print_formatted("Invalid page size encountered in vm_map_physical_memory: %x", page_size);
        HALT();
    }

    // Calculate which indices we should look at when setting up the mapping for this virtual page.
    int pml4_index = (virtual_page >> VM_PML4_INDEX_LOW_BIT) & VM_INDEX_MASK;
    int pdp_index = (virtual_page >> VM_PDP_INDEX_LOW_BIT) & VM_INDEX_MASK;
    int pd_index = (virtual_page >> VM_PD_INDEX_LOW_BIT) & VM_INDEX_MASK;
    int pt_index = (virtual_page >> VM_PT_INDEX_LOW_BIT) & VM_INDEX_MASK;

    if (!pml4[pml4_index].present)
    {
        // This PML4 entry is not present. We need to set it up.
        //
        // The PDP base address is the physical address with the lower 12 bits shifted off. In other words, it must be
        // page aligned and the "address" can really be seen as a physical 4 KiB page number.
        pml4[pml4_index].pdp_base_address = ((uint32_t)pdp + pml4_index * VM_SMALL_PAGE_SIZE) / VM_SMALL_PAGE_SIZE;

        // We enable write/through caching for this PML4 entry, since that takes precedence over all pages below it.
        pml4[pml4_index].pwt = 1;
        pml4[pml4_index].writable = 1;
        pml4[pml4_index].present = 1;
    }

    if (!pdp[pdp_index].present)
    {
        // This PDP entry is not present. We need to set it up.
        //
        // The logic for this address is the same as for the other tables.
        pdp[pdp_index].pd_base_address = ((uint32_t)pd + pdp_index * VM_SMALL_PAGE_SIZE) / VM_SMALL_PAGE_SIZE;

        // We enable write/through caching for this PML4 entry, since that takes precedence over all pages below it.
        pdp[pdp_index].pwt = 1;
        pdp[pdp_index].writable = 1;
        pdp[pdp_index].present = 1;
    }

    if (!pd[pd_index].present)
    {
        // Likewise for the page directory; if the entry is not present, set it up.

        // The logic for this address is the same as for the other tables. Note that this code is not >125 GiB-compliant for
        // the moment. We need to update it before the 125 GiB barrier is exceeded (see the MemoryMap for more
        // information). It should be pretty simple to fix actually, but I'll not put any time into it right now.
        pd[pd_index].pt_base_address = ((uint32_t)low_pt + pt_index * VM_SMALL_PAGE_SIZE) / VM_SMALL_PAGE_SIZE;

        // We enable write/through caching for this PML4 entry, since that takes precedence over all pages below it.
        pd[pd_index].pwt = 1;
        pd[pd_index].writable = 1;
        pd[pd_index].present = 1;
    }

    // ...and finally, the 4-level VM structures has come to its most fine-grained part: the page table. Here, we don't even
    // check the "present" flag since we reset it anyway. Other than that, the code is basically the same as above,
    // EXCEPT... one important exception: the page_base_address is obviously not set to Yet Another Paging Table<tm>, but
    // rather to the actual page itself (phew!)
    low_pt[pt_index].page_base_address = physical_page;

    low_pt[pt_index].pwt = 1;
    low_pt[pt_index].writable = 1;
    low_pt[pt_index].present = 1;
}

void vm_setup_paging_structures(uint64_t available_memory)
{
    // As mentioned in the vm.h file, the number of PDP tables is variable, depending on the amount of available RAM in the
    // machine. There will always be at least one PDP table, but if the amount of RAM in the machine gets above 512 GiB,
    // there will be multiple.
    int num_pdps = available_memory / (512 * GiB);

    // If the amount of memory in the machine is not evenly divisible with 512 * GiB, we need to take this into account when
    // doing these calculations.
    if (available_memory % (512 * GiB) != 0)
    {
        num_pdps++;
    }

    // Exactly the same theory can be applied to the number of PDs.
    int num_pds = available_memory / (1 * GiB);

    if (available_memory % (1 * GiB) != 0)
    {
        num_pds++;
    }

    pd = (pde_t *)((void *)pdp + VM_SMALL_PAGE_SIZE * num_pdps);
#ifdef VM_DEBUG
    io_print_formatted("PML4: %x\n", pml4);
    io_print_formatted("PDP base: %x\n", pdp);
    io_print_formatted("PD base: %x\n", pd);
    io_print_formatted("Low PT base: %x\n", low_pt);
    io_print_formatted("Sizeof pml4e_t: %u\n", sizeof(pml4e_t));
#endif

    // Start off by zapping the pages used for the PML4, PDP, PD and low page tables, just so we make sure they have
    // reasonable content.
    memory_zero(pml4, VM_SMALL_PAGE_SIZE);
    memory_zero(pdp, VM_SMALL_PAGE_SIZE * num_pdps);
    memory_zero(pd, VM_SMALL_PAGE_SIZE * num_pds);
    memory_zero(low_pt, VM_SMALL_PAGE_SIZE);

    if (available_memory < 2 * MiB)
    {
        io_print_formatted("Less than 2 MiB of RAM. This is not supported by cocOS. Halting.");
        HALT();
    }
    
    // OK, all tables are set up. We try to map up the low 2 MiB first. Since we want the first block of it unmapped (to be
    // able to trap NULL pointer references), we can't just map it using a "large page" (which is exactly 2 MiB
    // large). Instead, we need to map those two megs of RAM using small (4 KiB) pages.
    for (uint64_t page_number = 1; page_number < VM_ENTRIES_PER_PAGE; page_number++)
    {
        vm_map_physical_memory(page_number, page_number, VM_SMALL_PAGE_SIZE);
    }
}
