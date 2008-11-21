/*
 * $Id$
 *
 * vm.h - Common Virtual Memory definitions and structures.
 *
 * Author: Per Lundberg <per@halleluja.nu> 
 */

#ifndef __COMMON_VM_H__
#define __COMMON_VM_H__

#include "stdint.h"

////
//// Defines
////
// The size of a "small" page. (4096 bytes)
#define VM_SMALL_PAGE_SIZE              0x1000

// For the full specification/definition of the amd64 paging architecture, please read AMD64 Architecture Programmers Manual,
// Volume 2: System Programing.

// We place the VM structures for the kernel process at this memory address and upwards. For more information about the
// details about the reserved physical memory zones, please see the MemoryMap.txt file in the Documentation & Specifications
// folder.
//
// Something very important to note here is that since we place the PML4 at the very first page in memory (0x0-0xFFF). This
// memory will be "unmapped" in the paging structures being set up here (to be able to trap NULL-pointer references). This
// means that the kernel PML4 will not be accessible when paging has been turned on. This shouldn't really be a huge
// problem. Each problem will have its own PML4 anyway, and the kernel will set up mapping for the full amount of physical
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

////
//// Type definitions and structures
////
// A PML4 entry.
typedef struct
{
    // No-execute means that addresses in this page can not be executed.
    uint64_t no_execute: 1;

    // Available to the OS.
    uint64_t available1: 11;

    // The base-address of the PDP, shifted to the left 12 bits.
    uint64_t pdp_base_address: 40;

    // Available to the OS:
    uint64_t available2: 3;

    // The following bits should be set to zero.
    uint64_t reserved: 3;

    uint64_t available: 1;
    uint64_t pcd: 1;

    // Page-level Writethrough. Set = writethrough, clear = writeback caching.
    uint64_t pwt: 1;

    // If this flag is set, the page is accessible to user-level code.
    uint64_t user_level_accessible: 1;

    // If this flag is set, the page can be written to. If clear, the page is readonly.
    uint64_t writable: 1;
    
    // The accessed flag is set by the CPU when the page is being read from or written to. It can be used to implement
    // swapping, a feature we hope we will never need. :-)
    uint64_t accessed: 1;
} pml4e_t;

// A PDP entry.
typedef struct
{
    uint64_t dummy;
} pdpe_t;

// A PD entry.
typedef struct
{
    uint64_t dummy;
} pde_t;

// A PT entry.
typedef struct
{
    uint64_t dummy;
} pte_t;

#endif // !__VM_H__
