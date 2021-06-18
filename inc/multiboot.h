#pragma once

enum MultibootMemoryTypes {
    MultibootMemoryAvailable       = 1,
    MultibootMemoryReserved        = 2,
    MultibootMemoryAcpiReclaimable = 3,
    MultibootMemoryNVS             = 4,
    MultibootMemoryBadRAM          = 5,
};
