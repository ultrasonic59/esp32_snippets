#include <stdio.h>
#include "my_spi.h"

void init_my_spi(void)
{
#if 0
///====== CS_PIN ==========================
///gpio_reset_pin( CS_PIN );
///gpio_set_direction( CS_PIN, GPIO_MODE_OUTPUT );
///gpio_set_level( CS_PIN, 0 );
///====== DC_PIN ==========================
gpio_reset_pin( DC_PIN );
gpio_set_direction( DC_PIN, GPIO_MODE_OUTPUT );
gpio_set_level( DC_PIN , 0 );
#endif

///============GPIO_RESET=====================
#if 0
gpio_reset_pin( GPIO_RESET );
gpio_set_direction( GPIO_RESET, GPIO_MODE_OUTPUT );
gpio_set_level( GPIO_RESET, 1 );
delayMS(50);
gpio_set_level( GPIO_RESET, 0 );
delayMS(50);
gpio_set_level( GPIO_RESET, 1 );
delayMS(50);
#endif
}
