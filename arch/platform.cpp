#include "platform.h"
#include "types.h"

#ifdef ARCH_x86
#include "x86/statesx86.h"
#include "x86/timerx86.h"
#include "x86/videox86.h"
#include "x86/idtx86.h"
#include "x86/kbx86.h"
#include "x86/bochs.h"
#include "x86/vesa.h"
#include "x86/x86.h"
#include "x86/pcix86.h"
#include "x86/atax86.h"
#include "x86/taskx86.h"
#include "x86/randomx86.h"
#include "x86/cpux86.h"
#endif

#ifdef ARCH_ARM
#include "arm/idtarm.h"
#include "arm/statesarm.h"
#include "arm/videoarm.h"
#include "arm/timerarm.h"
#include "arm/armfb.h"
#include "arm/cpuarm.h"
#endif

#ifdef ARCH_LINUX
#include "linux/idtlinux.h"
#include "linux/timerlinux.h"
#include "linux/stateslinux.h"
#include "linux/videolinux.h"
#include "linux/tasklinux.h"
#include "linux/x11.h"
#include "linux/cpulinux.h"
#endif

static Timer *__platform_timer = nullptr;
static Video *__platform_video = nullptr;
static IDT   *__platform_idt   = nullptr;
static KB    *__platform_kb    = nullptr;
static FB    *__platform_fb    = nullptr;
static PCI   *__platform_pci   = nullptr;
static ATA   *__platform_ata   = nullptr;
static Task  *__platform_task  = nullptr;
static ProcessManager *__platform_pm = nullptr;
static VFS   *__platform_vfs   = nullptr;
static Random *__platform_random = nullptr;
static CPU *__platform_cpu     = nullptr;

Platform::Platform()
{
    current = PlatformNone;
    m_state = nullptr;

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
    m_state = nullptr;
    #endif

    #ifdef ARCH_LINUX
    current = PlatformLinux;
    m_state = new StateLinux();
    #endif
}

Platform::~Platform()
{
    if (m_state != nullptr) {
        delete m_state;
    }
    m_state = nullptr;
}

State *Platform::state()
{
    return m_state;
}

CPU *Platform::cpu()
{
    if (__platform_cpu == nullptr) {
        #ifdef ARCH_ARM
        __platform_cpu = new CPUARM();
        #endif
        #ifdef ARCH_x86
        __platform_cpu = new CPUX86();
        #endif
        #ifdef ARCH_LINUX
        __platform_cpu = new CPULinux();
        #endif
    }

    return __platform_cpu;
}

Timer *Platform::timer()
{
    if (__platform_timer == nullptr) {
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
    if (__platform_video == nullptr) {
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
    if (__platform_idt == nullptr) {
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
    if (__platform_kb == nullptr) {
        __platform_kb = new KBX86();
    }
    #endif
    return __platform_kb;
}

FB *Platform::fb()
{
    #ifdef FEATURE_GRAPHICS
    if (__platform_fb == nullptr) {
        #ifdef ARCH_ARM
        __platform_fb = new ARMFB();
        #endif
        #ifdef ARCH_x86
        //__platform_fb = new Vesa();
        __platform_fb = new BochsFB(__platform_pci);
        if (!__platform_fb->isValid()) {
            delete __platform_fb;
            __platform_fb = new Vesa();
        }
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
    if (__platform_pci == nullptr) {
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
    if (__platform_ata == nullptr) {
        __platform_ata = new ATAX86();
    }
    #endif
    #endif
    return __platform_ata;
}

Task *Platform::task()
{
    #ifdef ARCH_x86
    if (__platform_task == nullptr) {
        __platform_task = new TaskX86();
    }
    #endif
    #ifdef ARCH_LINUX
    if (__platform_task == nullptr) {
        __platform_task = new TaskLinux();
    }
    #endif
    return __platform_task;
}

ProcessManager *Platform::processManager()
{
    if (__platform_pm == nullptr) {
        __platform_pm = new ProcessManager();
    }
    return __platform_pm;
}

VFS *Platform::vfs()
{
    if (__platform_vfs == nullptr) {
        __platform_vfs = new VFS();
    }
    return __platform_vfs;
}

Random *Platform::random()
{
    #ifdef ARCH_x86
    if (__platform_random == nullptr) {
        __platform_random = new RandomX86();
    }
    #endif
    if (__platform_random == nullptr) {
        __platform_random = new Random();
    }
    return __platform_random;
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
