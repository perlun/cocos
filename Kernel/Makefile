#
# Top-level makefile in the kernel.
#
# Author: Per Lundberg <per@halleluja.nu>
# Copyright: © 2008-2009, 2013 Per Lundberg
#

UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
CC=/usr/local/gcc-4.7.2-for-linux64/bin/x86_64-pc-linux-gcc
TARGET_PATH=/Volumes/cocOS
endif

all:
	CC=$(CC) make -C 32bit_loader
	CC=$(CC) make -C 64bit_kernel

clean:
	make -C 32bit_loader clean
	make -C 64bit_kernel clean

install:
	CC=$(CC) TARGET_PATH=$(TARGET_PATH) make -C 32bit_loader install
	CC=$(CC) TARGET_PATH=$(TARGET_PATH) make -C 64bit_kernel install

