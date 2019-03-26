#include "acpi.h"

uint32_t acpiCpuCount()
{
    return 1;
}

uint32_t acpiCpuId(uint32_t)
{
    return 0;
}

uint32_t acpiCpuLocalId()
{
    return 0;
}

void acpiInitCpuById(uint32_t)
{
}

void acpiStartCpuById(uint32_t, uint32_t)
{
}

void acpiSearch()
{
}
