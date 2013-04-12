/* 
 * $Id$
 *
 * main.c - the main() method of the kernel. As you would expect, this is where the execution of the cosmOS kernel
 * begins.
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: © 2008, 2013 Per Lundberg
 */

#include "common/misc.h"
#include "cpu.h"
#include "io.h"
#include "multiboot.h"
#include "vm.h"

void main(multiboot_info_t *multiboot_info, uint64_t upper_memory_limit)
{
    io_init();
    io_leet_print("cocOS64 version 2013 loading...");
#ifdef CHANGESET
    io_print_line("Compiled from changeset " CHANGESET);
#endif
    io_print("\n");

    // Alright. We are now in 64-bit mode. However, for the moment only the lowest 2 megs of RAM are properly 1-to-1 mapped
    // (identity mapped), and can be accessed. This is set up in the 64-bit initialization code in the 32-bit
    // loader. (64bit.S) Now, it is time to create real VM structure (PML4, Page Directory Pointers and Page Tables) for
    // all the physical memory.

    // There is only one little problem: the physical memory size is located in the Multiboot structure, which can be placed
    // anywere in RAM, and thus be inaccessible to us. We solve it by having it provided to us as a parameter to this
    // function: upper_memory_limit. This parameter gives us the total amount of memory in the system. By using this
    // parameter, we know how much physical memory to map in the lower VM zone (see the cocOS VM Specification for more
    // information about the VM zones). When this memory is mapped, we can then read the Multiboot memory map once again to
    // know which pages to flag as usable and which ones that are reserved by hardware.
    
    vm_init (upper_memory_limit);

    io_print("Kernel command line: ");
    io_print(KERNEL_COMMAND_LINE);

    while (1 == 1);
}
