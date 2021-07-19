#include "realtimex86.h"
#include "port.h"

#define NMI_DISABLE_BIT 1


static inline uint8_t bcd(uint8_t val)
{
    return (val & 0xf) + (val >> 4) * 10;
}

Realtime::TimeInfo RealtimeX86::getTime() const
{
    TimeInfo info;

    // Seconds
    Port::out(0x70, (NMI_DISABLE_BIT << 7) | 0x00);
    info.seconds = bcd(Port::in(0x71));
    // Minutes
    Port::out(0x70, (NMI_DISABLE_BIT << 7) | 0x02);
    info.minutes = bcd(Port::in(0x71));
    // Hours
    Port::out(0x70, (NMI_DISABLE_BIT << 7) | 0x04);
    info.hours = bcd(Port::in(0x71));

    // Day of month
    Port::out(0x70, (NMI_DISABLE_BIT << 7) | 0x07);
    info.day = bcd(Port::in(0x71));

    // Month
    Port::out(0x70, (NMI_DISABLE_BIT << 7) | 0x08);
    info.month = bcd(Port::in(0x71));

    // Year
    Port::out(0x70, (NMI_DISABLE_BIT << 7) | 0x09);
    info.year = bcd(Port::in(0x71));
    // Century
    Port::out(0x70, (NMI_DISABLE_BIT << 7) | 0x32);
    info.year += bcd(Port::in(0x71)) * 100;

    return info;
}
