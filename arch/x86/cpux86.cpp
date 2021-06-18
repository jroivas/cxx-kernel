#include "cpux86.h"
#include "types.h"
#include "platform.h"
#include "string.hh"
#include "gdt.h"
#include "acpi.h"

static uint32_t active_cpu_count = 0;

#define SMP_INIT_ADDR 0x2000
#define BASE_ADDR 0x8000

extern uint8_t smp_init;
extern uint8_t smp_init_end;

void initSMP_CPUS(Platform *platform)
{
    uint32_t id = acpiCpuLocalId();
    ptr_t *active_cpu_lock = (ptr_t *)SMP_INIT_ADDR;
    *active_cpu_lock = 0;
    (void)platform;

    Platform::video()->printf("Initial CPU id: %u\n", id);
    for (uint32_t i = 0; i < acpiCpuCount(); ++i) {
        if (acpiCpuId(i) != id) acpiInitCpuById(acpiCpuId(i));
    }

    Platform::timer()->msleep(10);

    // Copy SMP CPU init code into place
    Mem::copy((void*)BASE_ADDR, &smp_init, &smp_init_end - &smp_init + 1);
    active_cpu_count = 1;

    // Start CPUs
    for (uint32_t i = 0; i < acpiCpuCount(); ++i) {
        if (acpiCpuId(i) == id) continue;
        *active_cpu_lock = 0;

        acpiStartCpuById(acpiCpuId(i), BASE_ADDR/0x1000);
        Platform::timer()->msleep(10);
        int cnt = 0;
        while (*active_cpu_lock == 0 && cnt < 1000) {
            Platform::timer()->msleep(2);
            ++cnt;
        }
        /* Timer hit, try again */
        if (cnt >= 1000) {
            acpiStartCpuById(acpiCpuId(i), BASE_ADDR/0x1000);
            cnt = 0;
            while (*active_cpu_lock == 0 && cnt < 1000) {
                Platform::timer()->msleep(2);
                ++cnt;
            }
        }
        if (cnt < 1000) active_cpu_count++;
        else {
            Platform::video()->printf("Failed to boot cpu: %u\n",
                    acpiCpuId(i));
        }
        Platform::timer()->msleep(100);
    }

    Platform::timer()->msleep(100);
    //Platform::timer()->sleep(1);

    Platform::video()->printf("Total CPUs active: %u\n", active_cpu_count);
    Platform::video()->printf("Inited\n");
}

void CPUX86::initSMP(Platform *platform)
{
    acpiSearch();

    Platform::video()->printf("Cpu count: %u\n", acpiCpuCount());
    initSMP_CPUS(platform);
}
