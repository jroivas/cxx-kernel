#include "types.h"
#include "kernel.h"
#include "kernel_version.h"
#include "idt.h"
#include "timer.h"
#include "kb.h"
#include "mm.h"
#include "fb.h"
#include "ata.hh"
#include "processmanager.h"
#include "task.h"
#include "syscall.hh"
#include "fs/vfs.hh"
#include "fs/devfs.hh"
#include "fs/clothesfs.hh"
#include "fs/ustar.hh"

#ifdef ARCH_LINUX
#include "arch/linux/virtualdisc.h"
#endif

extern "C" int app_main(int argc, char **argv);
void app_proc()
{
#if 0
    int argc = 2;
    char *appname = (char*)MM::instance()->alloc(4);
    char *fname = (char*)MM::instance()->alloc(32);
    Mem::copy(appname, "app", 4);
    Mem::copy(fname, "/ustar/hello.py", 15);
    char * argv[] = {
        appname,
        fname,
        nullptr
    };
#else
    int argc = 1;
    char *appname = (char*)MM::instance()->alloc(4);
    Mem::copy(appname, "app", 4);
    char * argv[] = {
        appname,
        nullptr
    };
#endif

    int res = app_main(argc, argv);
    Platform::video()->printf("\n\nApplication exit with code: %d\nHalting...\n", res);

    MM::instance()->free(appname);

    while (true) {
        //Timer::get()->wait(500);
    }
}

void kernel_loop()
{
    ProcessManager *pm = Platform::processManager();

    Task *a_task = Platform::task()->create((ptr_val_t)&app_proc, 0, 0);
    a_task->setName("Application main");
    pm->addTask(a_task);
    a_task->setPriority(1);

    while (1) {
#ifdef FEATURE_GRAPHICS
        if (Platform::fb() != nullptr && Platform::fb()->isConfigured()) {
            Platform::fb()->swap();
            Platform::fb()->blit();
        }
#endif

        /* Update screen */
        Timer::get()->wait(10);
    }
}

Kernel::Kernel(Platform *_platform, MultibootInfo *multiboot)
{
    platform = _platform;
    //paging_init(multiboot);
    video = Video::get();

    if (IDT::get() == nullptr) {
        delete platform;
        delete video;
        video = nullptr;
        platform = nullptr;
        return;
    }
    IDT::get()->initISR();
    IDT::get()->initIRQ();

    if (Timer::get() == nullptr) {
        delete platform;
        delete video;
        video = nullptr;
        platform = nullptr;
        return;
    }
    vfs = nullptr;
    pcidev = nullptr;

    paging_init(multiboot);

    Timer::get()->setFrequency(KERNEL_FREQUENCY);
    platform->state()->startInterrupts();
    platform->cpu()->initSMP(platform);

    KB::get();
}

Kernel::~Kernel()
{
    delete video;
    video = nullptr;
}

// XXX
#ifdef ARCH_ARM
#include "arch/arm/gpio.h"
#endif

void Kernel::initVideo()
{
    if (video != nullptr) {
        video->clear();

        initFrameBuffer();

        video->printf("Ticks: %lu!\n", Timer::get()->getTicks());

        video->printf("\nC++ kernel %s!!!\n\n", kernel_version);
    }
}

void Kernel::initFileSystem()
{
#if FEATURE_STORAGE
    vfs = Platform::vfs();
    vfs->register_filesystem(new DevFS);
    vfs->mount("/dev", "DevFS", "");
#endif
}

void Kernel::initPCI()
{
#ifdef FEATURE_STORAGE
    PCI *pcidev = Platform::pci();
    if (pcidev != nullptr) {
        if (pcidev->isAvailable()) video->printf("Found PCI\n");
        else video->printf("PCI not available\n");
        //pcidev->setVerbose();
        pcidev->scanDevices();
    }
#endif
}

void Kernel::initATA(Filesystem *cfs)
{
    (void)cfs;
#ifdef FEATURE_STORAGE
    ATA *ata = Platform::ata();
    if (ata != nullptr) {
        ata->init();
        ATA::Device *dev = ata->getDevice();
        bool mounted = false;
        while (dev != nullptr) {
            video->printf("dev %x... %lu\n", dev, ata->deviceSize(dev));
            if ((ata->deviceModel(dev) == ATA::STORAGE_SATA
                || ata->deviceModel(dev) == ATA::STORAGE_PATA)
                && ata->deviceSize(dev) > 0) {
                if (!mounted) {
                    video->printf("Mounting...\n");
                    ATAPhys *ataphys = new ATAPhys(dev);
                    cfs->setPhysical(ataphys);
                    vfs->register_filesystem(cfs);
                    //vfs->mount("/cfs", "ClothesFS", "");
                    vfs->mount("/ustar", "Ustar", "");
                    mounted = true;
                    video->printf("Mounted\n");
                }
                break;
            }

            dev = ata->nextDevice(dev);
        }
    }
#endif
}

void Kernel::initVirtualDisc()
{
#ifdef ARCH_LINUX
#ifdef FEATURE_STORAGE
    VirtualDisc *vd = new VirtualDisc();
    vd->append("test.img");
    ATA::Device *dev = vd->getDevice();
    if (dev != nullptr) {
        uint8_t buffer[512];
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
}

void Kernel::drawTestData()
{
#ifdef FEATURE_GRAPHICS
    platform->fb()->swap();
    platform->fb()->blit();
#if 1
    for (int j=0; j<120; j++) {
        platform->fb()->putPixel(j,10,255,255,255);
    }
    platform->fb()->swap();
    platform->fb()->blit();
#if 1
    for (int j=100; j<120; j++) {
        for (int i=100; i<200; i++) {
            platform->fb()->putPixel(i,100+j,255,0,0);
            platform->fb()->putPixel(j,100+i,0,0,255);
        }
    }
#endif
#endif
#endif
}

int Kernel::initFrameBuffer()
{
#ifdef FEATURE_GRAPHICS
    FB::ModeConfig conf;
    conf.width = 800;
    conf.height = 600;
    conf.depth = 24;

    if (platform->fb() == nullptr) return 1;

    platform->fb()->initialize();
    if (!platform->fb()->isValid()) {
        int idx = 1;
        FB *f;

        /* Try alternative framebuffers */
        do {
            f = platform->fbAlternative(idx);
            if (f == nullptr)
                return 1;
            idx++;
            f->initialize();
        } while (!f->isValid());
    }

    FB::ModeConfig *vconf = platform->fb()->query(&conf);
    if (vconf != nullptr) {
        video->adapt(vconf->width, vconf->height);
        platform->fb()->configure(vconf);

        // TODO: Can be removed
        drawTestData();
#if 0
        video->printf("C++ test kernel\n");
        platform->fb()->swap();
        platform->fb()->blit();
#endif
    }
#endif
    return 0;
}

void Kernel::startProcessManager()
{
    ProcessManager *pmanager = Platform::processManager();
    if (pmanager != nullptr && Platform::task() != nullptr) {

        Task *kernel_task = Platform::task()->create((ptr_val_t)kernel_loop, 0, 0);
        if (kernel_task != nullptr) {
            kernel_task->setSize(2);
            kernel_task->setNice(40);

            pmanager->setRunning();
            pmanager->addTask(kernel_task);
        }
    }
}

void Kernel::initSysCall()
{
    sys = new SysCall();
}

int Kernel::run()
{
    if (platform == nullptr) {
        return 1;
    }
    if (platform->state() == nullptr) {
        return 1;
    }
    //platform->state()->startInterrupts();

    initVideo();

    initFileSystem();

    initPCI();

#if FEATURE_STORAGE
#if 0
    ClothesFilesystem *cfs = new ClothesFilesystem;
#else
    UstarFilesystem *cfs = new UstarFilesystem;
#endif

    initATA(cfs);
#endif

    initVirtualDisc();

    initSysCall();

    /* Now everything should be set up, so we can safely start multitasking
    */
    startProcessManager();

    //TODO: Setup initial task
    while(1) {}

    return 0;
}
