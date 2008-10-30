#
# $Id$
#
# This is the makefile for the 64-bit cosmos kernel.
# Author: Per Lundberg <per@halleluja.nu>
#

TARGET_PATH = /var/lib/tftpboot
CC = gcc-4.1
INSTALL = install
AS_FLAGS = -c -m64 -Wall -Werror -Wno-main
CFLAGS = $(AS_FLAGS) --std=gnu99
LDFLAGS = -m64 -nostdlib -Wl,--oformat -Wl,binary -Wl,-N -Wl,-Ttext -Wl,80000 -e main

# No user-serviceable parts below this line. :-)

LINK = $(CC)
KERNEL = cocOS64.bin
KERNEL_OBJS = main.o io.o

all: $(KERNEL)

$(KERNEL): $(KERNEL_OBJS)
	$(LINK) $(LDFLAGS) $(KERNEL_OBJS) -o $(KERNEL)

clean:
	rm -f $(KERNEL) $(KERNEL_OBJS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -o $(@) $<

%.o: %.S Makefile
	$(CC) $(AS_FLAGS) -o $(@) $<

install: all
	$(INSTALL) $(KERNEL) $(TARGET_PATH)
