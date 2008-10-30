/*
 * $Id$
 *
 * main.c - C entry point for the 32-bit kernel loader.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 */

#include <stdint.h>

#include "64bit.h"
#include "io.h"
#include "multiboot.h"

#define HALT()    while (1 == 1)

/*
 * This is where execution starts when the entry point code in start.S has finished setting the most fundamental parts up. 
 *
 * @param magic The magic number provided to us from the boot loader. 
 * @param multiboot_info The multiboot info, provided to us from the boot loader.
 */
void main (uint32_t magic, multiboot_info_t *multiboot_info)
{
    io_init();
    io_print_line("cocOS32 version 2008 loading...");

    if (magic != 0x2BADB002)
    {
        io_print_line("Invalid multiboot signature received from boot loader. Possible memory fault.");
        io_print_line("Could also be boot loader bug. Kernel halted.");
        HALT();
    }

    // The first module loaded by the Multiboot loader is presumed to be the real (64-bit) kernel. We move it to its fixed
    // place in memory.
    if (multiboot_info->flags.has_module_info)
    {
        multiboot_module_info_t *module_info = (multiboot_module_info_t *) multiboot_info->modules_info;

        // An extremely inefficient way to do a memcpy(), but it works... :-)
        uint8_t *kernel = (uint8_t *) _64BIT_KERNEL_ENTRY_POINT;
        uint8_t *module = (uint8_t *) module_info->start;
        for (int i = 0; i < module_info->end - module_info->start; i++)
        {
            kernel[i] = module[i];
        }
    }
    else
    {
        io_print_line("No 64-bit kernel loaded. Kernel halted. You MUST specify a 64-bit kernel as the");
        io_print_line("first \"module\" in the Multiboot boot loader, for cocOS to function properly.");
        HALT();
    }

    // Alright, let's get moving. What we do now is set up basic data structures to be able to activate the ultra-cool amd64
    // "long mode". :-) But first, we will detect that the CPU is actually a 64-bit CPU.

    // Initially, I had thought about doing it in a bunch of C functions but it's actually more elegant to do it all in one
    // single assembly function, IMO.

    // Because of its nature, this function will never return. If 64-bit initialization fails, it will halt the CPU. 
    _64bit_init(multiboot_info);
}
