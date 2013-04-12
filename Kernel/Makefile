#
# Top-level makefile in the kernel.
#
# Author: Per Lundberg <per@halleluja.nu>
# Copyright: © 2008-2009, 2013 Per Lundberg
#

all:
	make -C 32bit_loader
	make -C 64bit_kernel

clean:
	make -C 32bit_loader clean
	make -C 64bit_kernel clean

install:
	make -C 32bit_loader install
	make -C 64bit_kernel install
