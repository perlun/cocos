#
# $Id$
#
# Top-level makefile in the kernel.
# Author: Per Lundberg <per@halleluja.nu>

UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
CC=/usr/local/gcc-4.7.2-for-linux64/bin/x86_64-pc-linux-gcc
endif

all:
	CC=$(CC) make -C 32bit_loader
	CC=$(CC) make -C 64bit_kernel

clean:
	make -C 32bit_loader clean
	make -C 64bit_kernel clean

install:
	make -C 32bit_loader install
	make -C 64bit_kernel install

