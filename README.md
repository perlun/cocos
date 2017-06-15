[![Build Status](https://travis-ci.org/perlun/cocos.svg?branch=master)](https://travis-ci.org/perlun/cocos)

# README

Welcome to cocOS. This is not intended to be a "real" operating system, which it indeed will never be; writing an operating system (let alone the kernel) is a full-time job for a number of people for a significant number of months, if not years. Rather, this is my personal playground, toying around with the 64-bit architecture just for fun.

# Development
## Prerequisites

- Vagrant
- Knowledge about how to copy-paste commands into a terminal. :wink:

## Setting up the development environment

```shell
$ vagrant up
$ vagrant ssh
```

## Building the kernel

```shell
$ cd Kernel
$ make
$ make install # Will update floppy.img
```

The result is that the `floppy.img` floppy disk image will get updated with the 32-bit loader and the 64-bit kernel of the cocOS system. You can mount this image in a virtualization software (like VirtualBox), and you should be able to boot the system. (It doesn't do much useful yet, apart from printing a message that it has been started.)
