#
# $Id$
#
# Top-level makefile in the kernel.
# Author: Per Lundberg <per@halleluja.nu>

all:
	make -C 32bit_loader
	make -C 64bit_kernel

install:
	make -C 32bit_loader install
	make -C 64bit_kernel install

