#include "platform.h"
#include "types.h"

#ifdef ARCH_x86
#include "x86/statesx86.h"
#include "x86/timerx86.h"
#include "x86/videox86.h"
#include "x86/idtx86.h"
#include "x86/kbx86.h"
#include "x86/vesa.h"
#include "x86/x86.h"
#include "x86/pcix86.h"
#include "x86/atax86.h"
#include "x86/taskx86.h"
#endif

#ifdef ARCH_ARM
#include "arm/idtarm.h"
#include "arm/statesarm.h"
#include "arm/videoarm.h"
#include "arm/timerarm.h"
#include "arm/armfb.h"
#endif

#ifdef ARCH_LINUX
#include "linux/idtlinux.h"
#include "linux/timerlinux.h"
#include "linux/stateslinux.h"
#include "linux/videolinux.h"
#include "linux/x11.h"
#endif

static Timer *__platform_timer = NULL;
static Video *__platform_video = NULL;
static IDT   *__platform_idt   = NULL;
static KB    *__platform_kb    = NULL;
static FB    *__platform_fb    = NULL;
static PCI   *__platform_pci   = NULL;
static ATA   *__platform_ata   = NULL;
static Task  *__platform_task  = NULL;
static ProcessManager *__platform_pm = NULL;

Platform::Platform()
{
    #ifdef ARCH_ARM
    current = PlatformARM;
    m_state = new StateARM();
    #endif

    #ifdef ARCH_x86
    current = PlatformX86;
    m_state = new StateX86();
    #endif

    #ifdef ARCH_x86_64
    current = PlatformX86_64;
    m_state = NULL;
    #endif

    #ifdef ARCH_LINUX
    current = PlatformLinux;
    m_state = new StateLinux();
    #endif
}

Platform::~Platform()
{
    if (m_state!=NULL) delete m_state;
    m_state = NULL;
}

State *Platform::state()
{
    return m_state;
}

#if 0
int Platform::CAS(ptr_val_t volatile *m_ptr, int cmp, int set)
{
    #ifdef ARCH_x86
    int res = cmp;
    asm volatile(
        "lock; cmpxchgl %1,%2\n"
        "setz %%al\n"
        "movzbl %%al,%0"
        : "+a"(res)
        : "r" (set), "m"(*(m_ptr))
        : "memory"
        );

    return res;
    #endif

    if ((int)*m_ptr==cmp) {
        *m_ptr=set;
        return 1;
    }
    return 0;
}
#endif

Timer *Platform::timer()
{
    if (__platform_timer == NULL) {
        #ifdef ARCH_ARM
        __platform_timer = new TimerARM();
        #endif
        #ifdef ARCH_x86
        __platform_timer = new TimerX86();
        #endif
        #ifdef ARCH_LINUX
        __platform_timer = new TimerLinux();
        #endif
    }

    return __platform_timer;
}

Video *Platform::video()
{
    if (__platform_video == NULL) {
        #ifdef ARCH_ARM
        __platform_video = new VideoARM();
        #endif
        #ifdef ARCH_x86
        __platform_video = new VideoX86();
        #endif
        #ifdef ARCH_LINUX
        __platform_video = new VideoLinux();
        #endif
    }
    return __platform_video;
}

IDT *Platform::idt()
{
    if (__platform_idt == NULL) {
        #ifdef ARCH_ARM
        __platform_idt = new IDTARM();
        #endif
        #ifdef ARCH_x86
        __platform_idt = new IDTX86();
        #endif
        #ifdef ARCH_LINUX
        __platform_idt = new IDTLinux();
        #endif
    }
    return __platform_idt;
}

KB *Platform::kb()
{
    #ifdef ARCH_x86
    if (__platform_kb == NULL) {
        __platform_kb = new KBX86();
    }
    #endif
    return __platform_kb;
}

FB *Platform::fb()
{
    #ifdef FEATURE_GRAPHICS
    if (__platform_fb == NULL) {
        #ifdef ARCH_ARM
        __platform_fb = new ARMFB();
        #endif
        #ifdef ARCH_x86
        __platform_fb = new Vesa();
        #endif
        #ifdef ARCH_LINUX
        __platform_fb = new X11();
        #endif
    }
    #endif
    return __platform_fb;
}

PCI *Platform::pci()
{
    #ifdef FEATURE_PCI
    #ifdef ARCH_x86
    if (__platform_pci == NULL) {
        __platform_pci = new PCIX86();
    }
    #endif
    #endif
    return __platform_pci;
}

ATA *Platform::ata()
{
    #ifdef FEATURE_STORAGE
    #ifdef ARCH_x86
    if (__platform_ata == NULL) {
        __platform_ata = new ATAX86();
    }
    #endif
    #endif
    return __platform_ata;
}

Task *Platform::task()
{
    #ifdef ARCH_x86
    if (__platform_task == NULL) {
        __platform_task = new TaskX86();
    }
    #endif
    return __platform_task;
}

ProcessManager *Platform::processManager()
{
    if (__platform_pm == NULL) {
        __platform_pm = new ProcessManager();
    }
    return __platform_pm;
}

void Platform::halt()
{
    #ifdef ARCH_x86
    hlt();
    #endif
}

void Platform::seizeInterrupts()
{
    #ifdef ARCH_x86
    cli();
    #endif
}

void Platform::continueInterrupts()
{
    #ifdef ARCH_x86
    sti();
    #endif
}
