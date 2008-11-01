/* 
 * $Id$
 *
 * main.c - the main() method of the kernel. As you would expect, this is where the execution of the cosmos kernel
 * begins. For political reasons (gcc complains at us), we can't just call this method main() though, since we use
 * different arguments than the traditional C main()...
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008 Per Lundberg
 */

#include "cpu.h"
#include "io.h"
#include "multiboot.h"

void main(multiboot_info_t *multiboot_header)
{
    io_init();
    io_leet_print("cocOS64 version 2008 loading...");
    io_print("\n");

    // This doesn't work: the Multiboot header parameter is correct, but it is usually put at the very end of the RAM and the
    // paging code in the 32-bit boot loader only maps the very first 2 megs of physical memory. So, we would have to set up
    // proper paging of the full physical memory before we try to access any of the Multiboot structures.

    // The only problem is: we would really prefer to do it the "Windows NT" :-) way... to map the full physical memory from
    // 2^48 (where 48 is the number if virtual address lines) and downwards. Of course, we can set up something temporary
    // first and then change later, but it is better to do it right from the beginning...

    // I think we do it like this: we try to get the mapping of the physical memory "docked" to the top of the virtual
    // address space first. It we can get it working (which we should), we can then access the Multiboot structures by adding
    // PHYSICAL_MEMORY_BASE to the pointers. We'll have to do similar things everytime we want to access a physical memory
    // address anyway.

    //io_print_formatted("[64] mbi: %X\n", multiboot_header);
    //io_print((const char *) (uint64_t) multiboot_header->command_line);
    //    io_print("\n");

    while (1 == 1);
}
