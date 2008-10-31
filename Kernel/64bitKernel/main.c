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

#include "io.h"
#include "multiboot.h"

void main(multiboot_info_t *multiboot_header)
{
    io_init();
    io_leet_print("cocOS64 version 2008 loading...");
    io_print("\n");

    io_print_formatted("%X\n", multiboot_header);

    while (1 == 1);
}
