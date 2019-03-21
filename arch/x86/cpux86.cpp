#include "cpux86.h"
#include "types.h"
#include "platform.h"
#include "string.hh"
#include "gdt.h"

static uint32_t acpi_addr = 0;
static uint32_t local_apic_id = 0x0020;

#define INTERRUPT_COMMAND_LOW 0x0300
#define INTERRUPT_COMMAND_HIGH 0x0310
#define ACPI_ID_SHIFT 24

// Delivery Mode
#define ICR_FIXED                       0x00000000
#define ICR_LOWEST                      0x00000100
#define ICR_SMI                         0x00000200
#define ICR_NMI                         0x00000400
#define ICR_INIT                        0x00000500
#define ICR_STARTUP                     0x00000600

// Destination Mode
#define ICR_PHYSICAL                    0x00000000
#define ICR_LOGICAL                     0x00000800

// Delivery Status
#define ICR_IDLE                        0x00000000
#define ICR_SEND_PENDING                0x00001000

// Level
#define ICR_DEASSERT                    0x00000000
#define ICR_ASSERT                      0x00004000

// Trigger Mode
#define ICR_EDGE                        0x00000000
#define ICR_LEVEL                       0x00008000

// Destination Shorthand
#define ICR_NO_SHORTHAND                0x00000000
#define ICR_SELF                        0x00040000
#define ICR_ALL_INCLUDING_SELF          0x00080000
#define ICR_ALL_EXCLUDING_SELF          0x000c0000

static const uint32_t max_cpu_count = 64; // FIXME
static uint32_t cpu_count = 0;
static uint32_t active_cpu_count = 0;
static uint32_t cpu_ids[max_cpu_count];

typedef struct AcpiHeader {
    uint32_t signature;
    uint32_t length;
    uint8_t mp_specification_revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} __attribute__((__packed__)) AcpiHeader;

typedef struct AcpiMadt
{
    AcpiHeader header;
    uint32_t localApicAddr;
    uint32_t flags;
} __attribute__((__packed__)) AcpiMadt;

typedef struct ApicHeader
{
    uint8_t type;
    uint8_t length;
} __attribute__((__packed__)) ApicHeader;

typedef struct ApicLocalApic
{
    ApicHeader header;
    uint8_t acpiProcessorId;
    uint8_t apicId;
    uint32_t flags;
} __attribute__((__packed__)) ApicLocalApic;

static inline uint32_t mmio_read_32(void *ptr)
{
    return *(volatile uint32_t *)(ptr);
}

static inline void mmio_write_32(void *ptr, uint32_t data)
{
    *(volatile uint32_t *)(ptr) = data;
}

static inline uint32_t acpi_read(uint32_t id)
{
    return mmio_read_32((void*)(acpi_addr + id));
}

static inline void acpi_write(uint32_t id, uint32_t data)
{
    return mmio_write_32((void*)(acpi_addr + id), data);
}

uint32_t localId()
{
    return acpi_read(local_apic_id) >> ACPI_ID_SHIFT;
}

static int parseAcpiApic(uint32_t addr)
{
    struct AcpiMadt *madt = (AcpiMadt *)addr;

    uint8_t *ptr = (uint8_t*)(madt + 1);
    uint8_t *end = (uint8_t*)((uint8_t*)madt + madt->header.length);

    acpi_addr = madt->localApicAddr;
	Platform::video()->printf("laaa %u\n", madt->localApicAddr);
	while (ptr < end) {
		ApicHeader *header = (ApicHeader *)ptr;
		if (header->type == 0) {
			ApicLocalApic *local = (ApicLocalApic *)ptr;
			if (cpu_count < max_cpu_count) {
				cpu_ids[cpu_count++] = local->apicId;
			}
		}
		else if (header->type == 1) {
	        //Platform::video()->printf("HT 1 unimp\n");
		}
		else if (header->type == 2) {
	        //Platform::video()->printf("HT 2 unimp\n");
		}
		ptr += header->length;
	}

	return 0;
}

static int parseAcpiDT(uint32_t addr)
{
    struct AcpiHeader *header = (AcpiHeader *)addr;

    if (header->signature == 0x50434146) {
        Platform::video()->printf("ACPI FADT unimplemented!\n");
    } else if (header->signature == 0x43495041) {
		return parseAcpiApic(addr);
        Platform::video()->printf("ACPI acp unimplemented!\n");
    } else {
        Platform::video()->printf("Invalid HDR: %u\n", header->signature);
    }

    return 0;
}

static int parseRSDT(uint32_t addr)
{
    AcpiHeader *header = (AcpiHeader *)addr;
    uint32_t *ptr = (uint32_t*)(header + 1);
    uint32_t *end = (uint32_t*)((uint8_t*)header + header->length);

    while (ptr < end) {
        uint32_t hdr = *(ptr++);
        parseAcpiDT(hdr);
    }

    return 0;
}

static int parseACPI(uint8_t *ptr)
{
    uint8_t sum = 0;
    for (int i = 0; i < 20; ++i) sum += ptr[i];
    if (sum) return 1;

    char oem[7];
    Mem::copy(oem, ptr + 9, 6);
    oem[6] = 0;
    Platform::video()->printf("OEM: %s\n", oem);

    uint8_t rev = ptr[15];
    Platform::video()->printf("REV: %u\n", rev);
    if (rev == 0) {
        uint32_t addr = *(uint32_t *)(ptr + 16);
        parseRSDT(addr);
    } else if (rev == 1) {
        Platform::video()->printf("ACPI rev 1 unimplemented!");
    }

    return 0;
}

static void searchACPI()
{
    uint8_t *ptr = (uint8_t*)0x000e0000;
    uint8_t *end = (uint8_t*)0x000fffff;

    while (ptr < end) {
        uint64_t signature = *(uint64_t*)ptr;
        if (signature == 0x2052545020445352) {
            if (parseACPI(ptr)) break;
        }
        ptr += 16;
    }
}

static void init_CPU_ID(uint32_t id)
{
    acpi_write(INTERRUPT_COMMAND_HIGH, id << ACPI_ID_SHIFT);
    acpi_write(INTERRUPT_COMMAND_LOW, ICR_INIT | ICR_PHYSICAL | ICR_ASSERT |
            ICR_EDGE | ICR_NO_SHORTHAND);

    while (acpi_read(INTERRUPT_COMMAND_LOW) & ICR_SEND_PENDING);
}

static void start_CPU_ID(uint32_t id, uint32_t vector)
{
    acpi_write(INTERRUPT_COMMAND_HIGH, id << ACPI_ID_SHIFT);
    acpi_write(INTERRUPT_COMMAND_LOW, vector | ICR_STARTUP | ICR_PHYSICAL |
            ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);

    while (acpi_read(INTERRUPT_COMMAND_LOW) & ICR_SEND_PENDING);
}

#define SMP_INIT_ADDR 0x2000
#define BASE_ADDR 0x8000

extern uint8_t smp_init;
extern uint8_t smp_init_end;

void initSMP_CPUS(Platform *platform)
{
    uint32_t id = localId();
    ptr_t *active_cpu_lock = (ptr_t *)SMP_INIT_ADDR;
    *active_cpu_lock = 0;
    (void)platform;

    Platform::video()->printf("My local id: %u\n", id);
	for (uint32_t i = 0; i < cpu_count; ++i) {
        if (cpu_ids[i] != id) init_CPU_ID(cpu_ids[i]);
	}

    Platform::timer()->msleep(10);

    // Copy SMP CPU init code into place
    Mem::copy((void*)BASE_ADDR, &smp_init, &smp_init_end - &smp_init + 1);
    active_cpu_count = 1;

    // Start CPUs
	for (uint32_t i = 0; i < cpu_count; ++i) {
        if (cpu_ids[i] == id) continue;
        *active_cpu_lock = 0;

        start_CPU_ID(cpu_ids[i], 0x8);
        Platform::timer()->msleep(10);
        int cnt = 0;
        while (*active_cpu_lock == 0 && cnt < 1000) {
            Platform::timer()->msleep(2);
            ++cnt;
        }
        if (cnt >= 1000) {
            start_CPU_ID(cpu_ids[i], 0x8);
            cnt = 0;
            while (*active_cpu_lock == 0 && cnt < 1000) {
                Platform::timer()->msleep(2);
                ++cnt;
            }
        }
        if (cnt < 1000) active_cpu_count++;
        else {
            Platform::video()->printf("Failed to boot cpu: %u\n",
                    cpu_ids[i]);
        }
        Platform::timer()->msleep(10);
	}

    Platform::timer()->msleep(100);
    //Platform::timer()->sleep(1);

    Platform::video()->printf("Total CPUs active: %u\n", active_cpu_count);
    Platform::video()->printf("Inited\n");
}

void CPUX86::initSMP(Platform *platform)
{
    searchACPI();

    Platform::video()->printf("Cpu count: %u\n", cpu_count);
	initSMP_CPUS(platform);
}
