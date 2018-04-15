#include "gpio.h"

GPIO::GPIO(ptr_val_t base)
{
    gpio_base = base;
}

/* Write cmd to GPIO, use (base + index)
   WARNING This is potentially dangerous!
 */
void GPIO::write(ptr_val_t index, uint32_t cmd)
{
    *(ptr_t)(gpio_base+index) = cmd;
}
