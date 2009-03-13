/*
 * $Id$
 *
 * vm.h - Common Virtual Memory definitions and structures.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 * Copyright: (C) 2008-2009 Per Lundberg
 */

#ifndef __COMMON_VM_H__
#define __COMMON_VM_H__

#ifndef __ASSEMBLER__
#include <stdint.h>
#include "common/misc.h"
#endif

////
//// Defines
////

// The size of a "small" page, in bits and bytes. A small page is 4096 bytes big. (0x1000)
#define VM_SMALL_PAGE_BITS              (12)
#define VM_SMALL_PAGE_SIZE              (1 << VM_SMALL_PAGE_BITS)

// The size of a "large" page.
#define VM_LARGE_PAGE_SIZE              (2 * MiB)

// For the full specification/definition of the amd64 paging architecture, please read AMD64 Architecture Programmers Manual,
// Volume 2: System Programing.

// We place the VM structures for the kernel process at this memory address and upwards. For more information about the
// details about the reserved physical memory zones, please see the MemoryMap.txt file in the Documentation & Specifications
// folder.
//
// Something very important to note here is that since we place the PML4 at the very first page in memory (0x0-0xFFF). This
// memory will be "unmapped" in the paging structures being set up here (to be able to trap NULL-pointer references). This
// means that the kernel PML4 will not be accessible when paging has been turned on. This shouldn't really be a huge
// problem. Each process will have its own PML4 anyway, and the kernel will set up mapping for the full amount of physical
// memory available in the system -- in essence, each 512 GiB block of RAM will have its own PDP table. 
//
// The kernel PML4 is set up before paging has been activated, so the initial setup won't be a problem.
#define VM_STRUCTURES_BASE_ADDRESS      0x0

// The PML4 is placed first in the paging structures zone. There is always only one PML4 table. It handles bit 39-47 in the
// virtual address space. A PML4, just like a PDP, PD or PT is always 4 KiB large.
#define VM_STRUCTURES_PML4_ADDRESS      (VM_STRUCTURES_BASE_ADDRESS)

// After the PML4, we place the "first page table". This page table maps the very first 2 MiB of RAM in the machine. It would
// be more logical, in one sense, to place the PDP(s) and PD(s) (since the layout goes PML4 -> PDP table -> PD -> PT,
// but... Those other tables (PDP and PD) can have a variable size, depending on the RAM installed in the machine, but there
// will only always be exactly one page table here. So, in accordance with the KISS principle, we Keep It Simple by placing
// as many structures as possible here on fixed locations.
#define VM_STRUCTURES_LOW_PAGE_TABLE_ADDRESS \
                                        (VM_STRUCTURES_PML4_ADDRESS + VM_SMALL_PAGE_SIZE)

// After that comes the PDP (Page Directory Pointer) table. One PDP table can handle up to 2^39 bytes of RAM = 512 GiB. So,
// when the physical memory in the machine goes above this number, there will be multiple PDPs.
#define VM_STRUCTURES_PDP_ADDRESS       (VM_STRUCTURES_LOW_PAGE_TABLE_ADDRESS + VM_SMALL_PAGE_SIZE)

// Then, we have page directories. One page directory maps 1 gigabyte of RAM, in amd64 mode. So, there will be one of those
// for each gigabyte of RAM in the machine. The location of this structure is variable; it will be placed immediately after
// the PDP(s). The number of 4 KiB pages necessary to hold the page directories will also vary.
//
// Since the number of PDP tables can vary, the base address of the page directories is not a constant but is computed at
// runtime.

// The number of entries in the PML4, PDP, PD and PT tables, per page.
#define VM_ENTRIES_PER_PAGE             (4096 / 8)      // That's 512, for those of us who can't count. :-)

// The lowest bit of the PML4/PDP/PD/PT index in the virtual address, minus the size of a small page. This makes these
// constants suitable for use when working with page numbers (rather than base addresses).
#define VM_PML4_INDEX_LOW_BIT           (39 - VM_SMALL_PAGE_BITS)
#define VM_PDP_INDEX_LOW_BIT            (30 - VM_SMALL_PAGE_BITS)
#define VM_PD_INDEX_LOW_BIT             (21 - VM_SMALL_PAGE_BITS)
#define VM_PT_INDEX_LOW_BIT             (12 - VM_SMALL_PAGE_BITS)

// The binary mask for the indices (they are all 9 bits wide = 0-511).
#define VM_INDEX_MASK                   (0x1FF)

// The rest of the code is not interesting for assembly code.
#ifndef __ASSEMBLER__

////
//// Type definitions and structures
////
// A PML4 entry.
typedef struct
{
    // The "present" flag determines whether the PDP referenced to by this PML4 entry exists in the physical memory.
    uint64_t present: 1;
    
    // If this flag is set, the pages can be written to. If clear, the pages are readonly.
    uint64_t writable: 1;

    // If this flag is set, the page is accessible to user-level code.
    uint64_t user_level_accessible: 1;

    // Page-Level Writethrough. Set = writethrough, clear = writeback caching.
    uint64_t pwt: 1;

    // Page-Level Cache Disable. Set = disable caching for this page, clear = enable caching.
    uint64_t pcd: 1;
    
    // The accessed flag is set by the CPU when the page is being read from or written to. It can be used to implement
    // swapping, a feature we hope we will never need. :-)
    uint64_t accessed: 1;

    // The following bits should be set to zero.
    uint64_t reserved: 3;

    // Available to the OS:
    uint64_t available2: 3;

    // The base-address of the PDP, left-shifted 12 bits.
    uint64_t pdp_base_address: 40;

    // Available to the OS.
    uint64_t available1: 11;

    // No-execute means that addresses in this PML4E can not be executed.
    uint64_t no_execute: 1;
} pml4e_t;

// A PDP entry.
typedef struct
{
    // The "present" flag determines whether the PD referenced to by this PDP entry exists in the physical memory.
    uint64_t present: 1;
    
    // If this flag is set, the pages can be written to. If clear, the pages are readonly.
    uint64_t writable: 1;

    // If this flag is set, the pages are accessible to user-level code.
    uint64_t user_level_accessible: 1;

    // Page-Level Writethrough. Set = writethrough, clear = writeback caching.
    uint64_t pwt: 1;

    // Page-Level Cache Disable. Set = disable caching for this page, clear = enable caching.
    uint64_t pcd: 1;

    // The accessed flag is set by the CPU when the page is being read from or written to. It can be used to implement
    // swapping, a feature we hope we will never need. :-)
    uint64_t accessed: 1;

    // The following bits should be set to zero.
    uint64_t reserved: 3;

    // Available to the OS.
    uint64_t available2: 3;
    
    // The base-address of the PD, left-shifted 12 bits.
    uint64_t pd_base_address: 40;

    // Available to the OS.
    uint64_t available1: 11;

    // No-execute means that addresses in these pages can not be executed.
    uint64_t no_execute: 1;
} pdpe_t;

// A PD entry.
typedef struct
{
    // The "present" flag determines whether the PD referenced to by this PDP entry exists in the physical memory.
    uint64_t present: 1;
    
    // If this flag is set, the pages can be written to. If clear, the pages are readonly.
    uint64_t writable: 1;

    // If this flag is set, the pages are accessible to user-level code.
    uint64_t user_level_accessible: 1;

    // Page-Level Writethrough. Set = writethrough, clear = writeback caching.
    uint64_t pwt: 1;

    // Page-Level Cache Disable. Set = disable caching for this page, clear = enable caching.
    uint64_t pcd: 1;

    // The accessed flag is set by the CPU when the page is being read from or written to. It can be used to implement
    // swapping, a feature we hope we will never need. :-)
    uint64_t accessed: 1;

    // The following bits should be set to zero.
    uint64_t reserved: 3;

    // Available to the OS.
    uint64_t available2: 3;
    
    // The base-address of the PT, left-shifted 12 bits.
    uint64_t pt_base_address: 40;

    // Available to the OS.
    uint64_t available1: 11;

    // No-execute means that addresses in these pages can not be executed.
    uint64_t no_execute: 1;
} pde_t;

// A PT entry.
typedef struct
{
    // The "present" flag determines whether the page exists in the physical memory.
    uint64_t present: 1;
    
    // If this flag is set, the page can be written to. If clear, the pages are readonly.
    uint64_t writable: 1;

    // If this flag is set, the page is accessible to user-level code.
    uint64_t user_level_accessible: 1;

    // Page-Level Writethrough. Set = writethrough, clear = writeback caching.
    uint64_t pwt: 1;

    // Page-Level Cache Disable. Set = disable caching for this page, clear = enable caching.
    uint64_t pcd: 1;

    // The accessed flag is set by the CPU when the page is being read from or written to. It can be used to implement
    // swapping, a feature we hope we will never need. :-)
    uint64_t accessed: 1;

    // The dirty flag is set by the CPU when the page has been written to. It is never cleared by the CPU, so if we want to
    // use it, we might have to take care of that part ourselves.
    uint64_t dirty: 1;

    // The PAT has to do with caching. We don't use it for the moment.
    uint64_t page_attribute_table: 1;

    // Is this flag "global" (shared in all threads)?
    uint64_t global: 1;

    // Available to the OS.
    uint64_t available2: 3;
    
    // The base-address of the page, left-shifted 12 bits.
    uint64_t page_base_address: 40;

    // Available to the OS.
    uint64_t available1: 11;

    // No-execute means that addresses in this page can not be executed.
    uint64_t no_execute: 1;
} pte_t;

#endif // !__ASSEMBLER__

#endif // !__COMMON_VM_H__
