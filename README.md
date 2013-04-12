# README

Welcome to cocOS. This is not intended to be a "real" operating system, which it indeed will never be; writing an operating system (let alone the kernel) is a full-time job for a number of people for a significant number of months, if not years. Rather, this is my personal playground, toying around with the 64-bit architecture just for fun.

## Compiling the stuff on Mac OS X

The code has most recently been brought up to date with gcc:s current demands on Mac OS X. Can probably be made to compile quite easily from Linux also; feel free to send me a pull request if you've done that.

To compile the code on OSX, you need the "standard" Xcode + CLI tools, and the following apps:

* [ELF/Linux cross-compiler for OSX](http://crossgcc.rts-software.org/doku.php?id=compiling_for_linux). The 64-bit package is the one that you will need; it can compile our 32-bit parts also.

You obviously also need some form of [virtual machine](http://en.wikipedia.org/wiki/Virtual_machine) software. Parallels on Mac or VMware Player on Windows are two good (albeit [non-free](http://en.wikipedia.org/wiki/Proprietary_software)) virtualization platforms. Bochs and QEmu might be two free software options that could work also.
