/*
 * main.c - C entry point for the 32-bit kernel loader.
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: © 2008, 2013, 2017 Per Lundberg
 */

#include <stdint.h>

#include "common/misc.h"
#include "64bit.h"
#include "io32.h"
#include "math.h"
#include "multiboot32.h"
#include "string32.h"
#include "vm32.h"

/*
 * This is where execution starts when the entry point code in start.S has finished setting the most fundamental parts up.
 *
 * @param magic The magic number provided to us from the boot loader.
 * @param multiboot_info The multiboot info, provided to us from the boot loader.
 */
void main (uint32_t magic, multiboot_info_t *multiboot_info)
{
    io_init();
    io_print_line("cocOS32 version 0.1.0 loading...");

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

    // Before we move on, we want to determine the amount of memory available available in the machine. This is used by the
    // kernel to know how much physical memory to map. We do this to make life a bit simpler for the 64-bit kernel, since it
    // initially only has access to a limited part of the physical memory, before paging is properly set up.
    uint64_t available_memory = 0;
    if (multiboot_info->flags.has_memory_map)
    {
        unsigned int index = 0;
        while (index < multiboot_info->memory_map_length)
        {
            // Pointer arithmetics is always a bit dangerous, but this one should be safe: memory_map_address is defined as
            // an integer, so we won't get any weird unexpected semantics...
            multiboot_memory_map_t *memory_map = (multiboot_memory_map_t *) (multiboot_info->memory_map_address + index);
            if (memory_map->type == MULTIBOOT_MEMORY_MAP_TYPE_RAM)
            {
                uint64_t base_address = ((uint64_t) memory_map->base_address_high << 32) + memory_map->base_address_low;
                uint64_t length = ((uint64_t) memory_map->length_high << 32) + memory_map->length_low;
                available_memory = MAX(available_memory, base_address + length);

                // This can be handy when debugging so I'll leave it in the code, commented out.
                //io_print_formatted("Index: %u, Memory info: %X %X, type: %u\n", index, base_address, length, memory_map->type);
            }

            // The record size is the size of the record MINUS the record size field... which is a 32-bit integer. So, we
            // need to add 4 here to get it right.
            index += memory_map->size + 4;
        }
    }
    else if (multiboot_info->flags.has_memory_info)
    {
        // This is a bit poor; we might miss some memory by doing like this. The alternative would be to not do any
        // autodetection at all in this case and just halt (and perhaps let the memory size be overridable by means of a
        // kernel command line parameter).
        available_memory = (multiboot_info->memory_upper * 1024) + (1024 * 1024);
    }
    else
    {
        io_print_line("cocOS requires a Multiboot kernel that provides memory information. Halting.");
        HALT();
    }

    string_copy(KERNEL_COMMAND_LINE, (char *) multiboot_info->command_line);

    // Alright, let's get moving. What we do now is set up basic data structures to be able to activate the ultra-cool amd64
    // "long mode". :-) But first, we will need to detect that the CPU is actually a 64-bit CPU, and similar.
    //
    // Initially, I had thought about doing this 64-bit initialization in a bunch of C functions but it's actually more
    // elegant to do it all in one single assembly function, IMO (_64bit_init). The only thing we do in C (because of
    // convenience and code cleanness) is to set up the 4-level long mode paging structures, which is done by the function
    // below.
    vm_setup_paging_structures(available_memory);

    // We now have VM set up, so let's call the aforementioned 64-bit initialization function.
    //
    // Because of its nature, this function will never return. If 64-bit initialization fails, it will halt the CPU.
    _64bit_init(multiboot_info, available_memory);
}
