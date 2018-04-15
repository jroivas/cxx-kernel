#ifndef GPIO_H
#define GPIO_H

#include <types.h>

class GPIO 
{
public:
    GPIO(ptr_val_t base);
    void write(ptr_val_t index, uint32_t cmd);
private:
    ptr_val_t gpio_base;
};
#endif
