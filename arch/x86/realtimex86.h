#pragma once

#include "realtime.h"

class RealtimeX86 : public Realtime {
public:
    RealtimeX86() : Realtime() {}
    virtual ~RealtimeX86() {}
    virtual TimeInfo getTime() const;
};
