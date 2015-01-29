#include "types.h"
#include "kernel.h"
#include "kernel_version.h"
#include "idt.h"
#include "timer.h"
#include "kb.h"
#include "mm.h"
#include "fb.h"
#include "ata.h"
#include "processmanager.h"
#include "task.h"
#include "syscall.hh"
#include "fs/vfs.hh"
#include "fs/devfs.hh"

#ifdef ARCH_LINUX
#include "arch/linux/virtualdisc.h"
#endif

#if 0
void B_proc()
{
    Platform::video()->printf("v_proc pid %d\n",Platform::processManager()->pid());
    while (1) {
        Platform::video()->printf("B");
        Timer::get()->wait(100);
    }
}

void A_proc()
{
    uint32_t pid = Platform::processManager()->pid();
    Platform::video()->printf("a_proc pid %d %d\n",Platform::processManager()->pid(), pid);
#if 0
    ProcessManager *pm = Platform::processManager();
    if (pid==Platform::processManager()->pid()) {
        Task *t = pm->clone();
        (void)t;
        //if (t!=NULL) pm->addTask(t);
    }
#endif
    while (1) {
        Platform::video()->printf("A");
        Timer::get()->wait(50);
    }
}

void C_proc()
{
    Platform::video()->printf("c_proc pid %d\n",Platform::processManager()->pid());
#ifdef ARCH_x86
    uint32_t cnt=0;
    while (1) {
        Platform::video()->printf("C");
        if (++cnt % 5 == 0)  {
            int32_t res = 0;
            asm ("int $0x99": "=a"(res));
            Platform::video()->printf("C %x\n",res);
        }
        Timer::get()->wait(500);
    }
#endif
}
#endif

extern "C" int app_main();
void app_proc()
{
    int res = app_main();
    Platform::video()->printf("\n\nApplication exit with code: %d\nHalting...\n", res);

    while (true) {
        //Timer::get()->wait(500);
    }
}

void kernel_loop()
{
#if 1
    ProcessManager *pm = Platform::processManager();

    Task *a_task = Platform::task()->create((ptr_val_t)&app_proc, 0, 0);
    a_task->setName("Application main");
    pm->addTask(a_task);
    a_task->setPriority(40);

#if 0
    Task *a_task = Platform::task()->create((ptr_val_t)&A_proc, 0, 0);
    Task *b_task = Platform::task()->create((ptr_val_t)&B_proc, 0, 0);
    Task *c_task = Platform::task()->create((ptr_val_t)&C_proc, 0, 0);

    a_task->setName("A task");
    a_task->setSize(100);
    a_task->setPriority(40);

    b_task->setName("B task");
    b_task->setNice(10);

    c_task->setPriority(10);
    c_task->setName("C task");

    pm->addTask(a_task);
    pm->addTask(b_task);
    pm->addTask(c_task);
#endif
#endif

    uint32_t start = Platform::timer()->getTicks();
    (void)start;
    while (1) {
        //FIXME
#if 0
        uint32_t end = Platform::timer()->getTicks();
        Platform::video()->printf("Diff: %d\n",end-start);
#endif
#if 0
        ProcessManager *pm = Platform::processManager();
        pm->schedule();
#endif
#ifdef FEATURE_GRAPHICS
        if (Platform::fb()!=NULL && Platform::fb()->isConfigured()) {
                Platform::fb()->swap();
                Platform::fb()->blit();
        }
#endif

        /* Update screen */
        Timer::get()->wait(1);
    }
}

Kernel::Kernel()
{
    platform = new Platform();
    video = Video::get();

    if (IDT::get() == NULL) {
        delete platform;
        delete video;
        video = NULL;
        platform = NULL;
        return;
    }
    IDT::get()->initISR();
    IDT::get()->initIRQ();

    if (Timer::get() == NULL) {
        delete platform;
        delete video;
        video = NULL;
        platform = NULL;
        return;
    }

    Timer::get()->setFrequency(KERNEL_FREQUENCY);
    KB::get();
}

Kernel::~Kernel()
{
    delete video;
    video = NULL;
}

// XXX
#ifdef ARCH_ARM
#include "arch/arm/gpio.h"
#endif

int Kernel::run()
{
    if (platform == NULL) {
        return 1;
    }
    if (platform->state() == NULL) {
        return 1;
    }
    platform->state()->startInterrupts();

    if (video != NULL) {
        video->clear();
#if 0
        video->printf("Ticks: %lu!\n",Timer::get()->getTicks());
        video->printf("Hello world!\n");
#endif
        video->printf("\nC++ kernel %s!!!\n\n", kernel_version);
#if 0
        video->printf("And a TAB\t test!\n");
        video->printf("Removing letter A\bB and continuing.\n");
        video->printf("\b\b\bABCDEFG\n");
        video->printf("Ticks: %lu!\n",Timer::get()->getTicks());
#endif

#if 0
        /* Some random timing... */
        volatile int b = 5;
        for (int i=0; i<0x1FFFFFF; i++) { b = b + i; }
        video->printf("Ticks: %lu!\n",Timer::get()->getTicks());
        for (int i=0; i<0x1FFFFF; i++) { b = b + i; }
        video->printf("Ticks: %lu!\n",Timer::get()->getTicks());
#endif
    }

#ifdef FEATURE_STORAGE
    PCI *pcidev = Platform::pci();
    if (pcidev != NULL) {
        if (pcidev->isAvailable()) video->printf("Found PCI\n");
        else video->printf("PCI not available\n");
        pcidev->setVerbose();
        pcidev->scanDevices();
    }
    ATA *ata = Platform::ata();
    if (ata != NULL) {
        ata->init();
        ATA::Device *dev = ata->getDevice();
        while (dev != NULL) {
            if ((ata->deviceModel(dev) == ATA::STORAGE_SATA
                || ata->deviceModel(dev) == ATA::STORAGE_PATA)
                && ata->deviceSize(dev) > 0) {
                break;
            }
            dev = ata->nextDevice(dev);
        }
        uint8_t buffer[512];
        for (uint32_t cc=0; cc<512; cc++) {
            buffer[cc] = 0;
        }

        video->printf("Reading...\n");
        for (uint32_t sec=0; sec<6; sec++) {
            if (ata->read(dev, buffer, 1,  sec)) {
                video->printf("ATA: Ok %d. %x %x %x\n", sec, buffer[0], buffer[1], buffer[2]);
            }
        }
        video->printf("Done.\n");
    }

    VFS *vfs = Platform::vfs();
    vfs->register_filesystem(new DevFS);
    vfs->mount("/dev", "DevFS", "");
#endif

#ifdef ARCH_LINUX
#ifdef FEATURE_STORAGE
    VirtualDisc *vd = new VirtualDisc();
    vd->append("test.img");
    ATA::Device *dev = vd->getDevice();
    uint8_t buffer[512];
    if (dev != NULL) {
        video->printf("Reading...\n");
        for (uint32_t sec=0; sec<6; sec++) {
            if (vd->read(dev, buffer, 1,  sec)) {
                video->printf("VIRTUAL: Ok %d. %x %x %x\n", sec, buffer[0], buffer[1], buffer[2]);
            }
        }
        video->printf("Done.\n");
    }
#endif
#endif

#ifdef FEATURE_GRAPHICS
    FB::ModeConfig conf;
    conf.width=800;
    conf.height=600;
    conf.depth=24;

    //FB::ModeConfig *vconf = platform->fb()->query(NULL);
    if (platform->fb()==NULL) return 1;

    FB::ModeConfig *vconf = platform->fb()->query(&conf);
    if (vconf != NULL) {
        video->printf("FB3\n");
        platform->fb()->configure(vconf);
        video->printf("FB3.2\n");
        platform->fb()->swap();
        platform->fb()->blit();
#if 1
        for (int j=0; j<120; j++) {
            platform->fb()->putPixel(j,10,255,255,255);
        }
        platform->fb()->swap();
        platform->fb()->blit();
        video->printf("FB5\n");
#if 1
        for (int j=100; j<120; j++) {
            for (int i=100; i<200; i++) {
                platform->fb()->putPixel(i,100+j,255,0,0);
                platform->fb()->putPixel(j,100+i,0,0,255);
            }
        }
#endif
#endif
#if 1
        video->printf("C++ test kernel\n");
        platform->fb()->swap();
        platform->fb()->blit();
#endif
    }
#endif
    //video->printf("Done\n");

    SysCall *sys = new SysCall();
    (void)sys;

    ProcessManager *pm = Platform::processManager();
    if (pm != NULL && Platform::task() != NULL) {
        Task *kernel_task = Platform::task()->create((ptr_val_t)kernel_loop, 0, 0);
        kernel_task->setSize(2);
        kernel_task->setNice(40);

        pm->setRunning();
        pm->addTask(kernel_task);
    }

    //TODO: Setup initial task
    while(1) {}

    return 0;
}
