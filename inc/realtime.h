#pragma once

#include "types.h"

class Realtime {
public:
    class TimeInfo {
    public:
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours;

        uint8_t day;
        uint8_t month;
        uint32_t year;
    };

    Realtime() {}
    virtual ~Realtime() {}
    virtual TimeInfo getTime() const
    {
        /* Just return zeros */
        TimeInfo info;
        return info;
    }
};
