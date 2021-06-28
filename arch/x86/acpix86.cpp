#include "platform.h"
#include "types.h"

static uint32_t acpi_addr = 0;
static uint32_t local_apic_id = 0x0020;

const uint32_t max_cpu_count = 64; // FIXME
static uint32_t cpu_count = 0;
static uint32_t cpu_ids[max_cpu_count];

uint32_t acpiCpuCount()
{
    return cpu_count;
}

uint32_t acpiCpuId(uint32_t index)
{
    if (index >= max_cpu_count) return 0xffffffff;
    return cpu_ids[index];
}

#define ACPI_CLEAR_ERRORS      0x0280
#define INTERRUPT_COMMAND_LOW  0x0300
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


static inline uint32_t mmio_read_32(volatile void *ptr)
{
    return *(volatile uint32_t *)(ptr);
}

static inline void mmio_write_32(volatile void *ptr, uint32_t data)
{
    *(volatile uint32_t *)(ptr) = data;
}

static inline uint32_t acpi_read(uint32_t id)
{
    return mmio_read_32((volatile uint32_t*)(acpi_addr + id));
}

static inline void acpi_write_mask(uint32_t id, uint32_t data, uint32_t mask)
{

    *((volatile uint32_t*)(acpi_addr + id)) = (*((volatile uint32_t*)(acpi_addr + id)) & mask) | data; 
}

static inline void acpi_write(uint32_t id, uint32_t data)
{
    return mmio_write_32((volatile uint32_t*)(acpi_addr + id), data);
}


uint32_t acpiCpuLocalId()
{
    return acpi_read(local_apic_id) >> ACPI_ID_SHIFT;
}

static int parseAcpiApic(uint32_t addr)
{
    struct AcpiMadt *madt = (AcpiMadt *)addr;

    uint8_t *ptr = (uint8_t*)(madt + 1);
    uint8_t *end = (uint8_t*)((uint8_t*)madt + madt->header.length);

    acpi_addr = madt->localApicAddr;
    pagingMapKernel(acpi_addr);
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
        //Platform::video()->printf("ACPI FADT unimplemented!\n");
    } else if (header->signature == 0x43495041) {
        return parseAcpiApic(addr);
    } else {
        //Platform::video()->printf("Invalid HDR: 0x%x\n", header->signature);
    }

    return 0;
}

static int parseRSDT(uint32_t addr)
{
    pagingMapKernel(addr);
    AcpiHeader *header = (AcpiHeader *)addr;
    uint32_t *ptr = (uint32_t*)(header + 1);
    uint32_t *end = (uint32_t*)((uint8_t*)header + header->length);
    // FIXME map pages

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
    //Platform::video()->printf("OEM: %s\n", oem);

    uint8_t rev = ptr[15];
    //Platform::video()->printf("REV: %u\n", rev);
    if (rev == 0) {
        uint32_t addr = *(uint32_t *)(ptr + 16);
        parseRSDT(addr);
    } else if (rev == 1) {
        Platform::video()->printf("ACPI rev 1 unimplemented!");
    }

    return 0;
}

void acpiSearch()
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

void acpiInitCpuById(uint32_t id)
{
    acpi_write(ACPI_CLEAR_ERRORS, 0);
#if 0
    acpi_write(INTERRUPT_COMMAND_HIGH, id << ACPI_ID_SHIFT);
    acpi_write(INTERRUPT_COMMAND_LOW, ICR_INIT | ICR_PHYSICAL | ICR_ASSERT |
            ICR_EDGE | ICR_NO_SHORTHAND);
    while (acpi_read(INTERRUPT_COMMAND_LOW) & ICR_SEND_PENDING);
#else
    acpi_write_mask(INTERRUPT_COMMAND_HIGH, id << ACPI_ID_SHIFT, 0x00ffffff);
    acpi_write_mask(INTERRUPT_COMMAND_LOW, ICR_LEVEL | ICR_INIT | ICR_PHYSICAL | ICR_ASSERT |
            ICR_EDGE | ICR_NO_SHORTHAND, 0xfff00000);
#endif

    do { __asm__ __volatile__ ("pause" : : : "memory"); } while (acpi_read(INTERRUPT_COMMAND_LOW) & ICR_SEND_PENDING);

    acpi_write_mask(INTERRUPT_COMMAND_HIGH, id << ACPI_ID_SHIFT, 0x00ffffff);
    /* Deassert */
    acpi_write_mask(INTERRUPT_COMMAND_LOW, ICR_LEVEL | ICR_INIT, 0xfff00000);

    do { __asm__ __volatile__ ("pause" : : : "memory"); } while (acpi_read(INTERRUPT_COMMAND_LOW) & ICR_SEND_PENDING);
}

void acpiStartCpuById(uint32_t id, uint32_t startupCode)
{
#if 0
    acpi_write(INTERRUPT_COMMAND_HIGH, id << ACPI_ID_SHIFT);
    acpi_write(INTERRUPT_COMMAND_LOW, startupCode | ICR_STARTUP | ICR_PHYSICAL |
            ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);
#else
    acpi_write(ACPI_CLEAR_ERRORS, 0);
    acpi_write_mask(INTERRUPT_COMMAND_HIGH, id << ACPI_ID_SHIFT, 0x00ffffff);
    acpi_write_mask(INTERRUPT_COMMAND_LOW, startupCode | ICR_STARTUP | ICR_PHYSICAL |
            ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND, 0xfff00000);
#endif

    do { __asm__ __volatile__ ("pause" : : : "memory"); } while (acpi_read(INTERRUPT_COMMAND_LOW) & ICR_SEND_PENDING);
}

