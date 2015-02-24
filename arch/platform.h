#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"

#include "states.h"
#include "timer.h"
#include "video.h"
#include "idt.h"
#include "kb.h"
#include "fb.h"
#include "pci.hh"
#include "ata.hh"
#include "task.h"
#include "processmanager.h"
#include "atomic.hh"
#include "fs/vfs.hh"

#ifdef ARCH_x86
#include "x86/pagingx86.h"
#endif
#ifdef ARCH_ARM
#include "arm/pagingarm.h"
#endif
#ifdef ARCH_LINUX
#include "linux/paginglinux.h"
#endif

class Platform
{
public:
    Platform();
    ~Platform();
    State *state();
    static Timer *timer();
    static Video *video();
    static IDT *idt();
    static KB *kb();
    static FB *fb();
    static PCI *pci();
    static ATA *ata();
    static Task *task();
    static ProcessManager *processManager();
    static VFS *vfs();

    static inline int CAS(ptr_val_t volatile *m_ptr, int cmp, int set)
    {
        return Platform_CAS(m_ptr, cmp, set);
    }
    static void halt();
    static void seizeInterrupts();
    static void continueInterrupts();

private:
    /* Extend the platform enums when supported */
    enum Platforms {
        PlatformNone = 0,
        PlatformX86,
        PlatformX86_64,
        PlatformARM,
        PlatformLinux
    };
    Platforms current;
    State *m_state;
};

#endif
