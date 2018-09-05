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

For example:

    sudo apt-get install build-essential nasm cmake

To build for specific target, you need cross compiler.
For example i686-elf-gcc from here: https://github.com/lordmilko/i686-elf-tools
Download and install it to your path.

First clone this git repository, and update musl libc source code:

    ./3rdparty/musl/update_musl.sh

After that make a build folder. There you can run cmake:

    mkdir build
    cd build
    cmake .. -DPLATFORM=x86 -DARCH=i386

You need to define platform and architecture.
Current option for platform are: x86, ARM and LINUX.
For architecture you can choose at least: i386 and x86_64

x86 builds i386 or x86_64 binary, ARM build qemu supported versatilepb kernel,
and LINUX build native Linux binary, which can just be executed from shell.

There's preliminary support for different features, which can be disabled
or enabled from cmake configurations. So far such features are:

*   GRAPHICS - enable framebuffer
*   STORAGE - enable disk access
*   PCI - enable PCI support (STORAGE will automatically enable this)

By default these are disabled, but can easily set on by running cmake:

    cmake .. -DPLATFORM=x86 -DARCH=i386 -DSTORAGE=1 -DGRAPHICS=1


After cmake you can just build the kenrnel:

    make

To run it (usually qemu/kvm is required), just do:

    make run

## Applications

Currently there's couple of test applications which can be built with the kernel:

    cmake .. -DPLATFORM=x86 -DARCH=i386 -DSTORAGE=1 -DAPP=hello

This will build Hello world example app and work like expected.
Another example is a bit more complicated, it opens /dev/random and reads few random numbers,
prints them out and closes the file:

    cmake .. -DPLATFORM=x86 -DARCH=i386 -DSTORAGE=1 -DAPP=randomfile
