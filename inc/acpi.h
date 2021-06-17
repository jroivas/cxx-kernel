#pragma once

#include "types.h"

uint32_t acpiCpuLocalId();
void acpiInitCpuById(uint32_t id);
/**
 * Start CPU with startup code position
 * @id CPU id
 * @startupCode startup code position is startup code address
 * divided by 0x1000. For example 0x8000 comes 0x8
 */
void acpiStartCpuById(uint32_t id, uint32_t startupCode);
void acpiSearch();

uint32_t acpiCpuCount();
uint32_t acpiCpuId(uint32_t index);
