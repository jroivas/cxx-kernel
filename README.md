# C++ kernel

Aim is to make simple virtualized kernel, which runs applications mostly in kernel space.
Should be more or less compatible with POSIX and standard C in order to be able to port
applications to it.

Major design decisions to keep it simple and efficient:

1.  Target running inside virtual machine
2.  Run applications in kernel space
3.  Link applications to kernel, no ELF or binary loaders needed
4.  No need for system calls and to use interrupts, faster to call method inside kernel
5.  Implement everything in C++


## Building

You need C and C++ compiler, nasm and CMake.
First clone this git repository and make a build folder under it.
There you can run cmake:

    mkdir build
    cd build
    cmake .. -DARCH=x86

You need to define architecture. Current option are: x86, ARM and LINUX.
x86 builds i386 binary, ARM build qemu supported versatilepb kernel,
and LINUX build native Linux binary, which can just be executed from shell.

There's preliminary support for different features, which can be disabled
or enabled from cmake configurations. So far such features are:

*   GRAPHICS - enable framebuffer
*   STORAGE - enable disk access
*   PCI - enable PCI support (STORAGE will automatically enable this)

By default these are disabled, but can easily set on by running cmake:

    cmake .. -DARCH=x86 -DSTORAGE=1 -DGRAPHICS=1


After cmake you can just build the kenrnel:

    make

To run it (usually qemu/kvm is required), just do:

    make run
