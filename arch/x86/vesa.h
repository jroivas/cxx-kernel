#ifndef VESA_H
#define VESA_H

#include "fb.h"
#include "types.h"

class Vesa : public FB
{
public:
    Vesa();
    ~Vesa();

    FB::ModeConfig *query(FB::ModeConfig *prefer);
    virtual bool configure(ModeConfig *mode);
    virtual void clear();
    void blit();

private:
    bool getVESA(void *ptr);
    void setMode(ModeConfig *mode);
    int modeDiff(FB::ModeConfig *conf, FB::ModeConfig *cmp);
    void copyMode(FB::ModeConfig *dest, FB::ModeConfig *src);
};

#endif
