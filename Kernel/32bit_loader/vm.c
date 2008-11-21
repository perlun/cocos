/*
 * $Id$
 *
 * vm.c - Virtual Memory-related functions.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 */

#include "io.h"
#include "misc.h"
#include "common/vm.h"
#include "vm.h"

//// Paging structures.
pml4e_t *pml4 = (pml4e_t *)VM_STRUCTURES_PML4_ADDRESS;
pdpe_t *pdp = (pdpe_t *)VM_STRUCTURES_PDP_ADDRESS;
pde_t *pd;
pte_t *low_pt = (pte_t *)VM_STRUCTURES_LOW_PAGE_TABLE_ADDRESS;

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
    io_print_formatted("PML4: %x\n", pml4);
    io_print_formatted("PDP base: %x\n", pdp);
    io_print_formatted("PD base: %x\n", pd);
    io_print_formatted("Low PT base: %x\n", low_pt);
    io_print_formatted("Sizeof pml4e_t: %u\n", sizeof(pml4e_t));
}
