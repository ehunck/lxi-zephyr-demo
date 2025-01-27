#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

#include <zephyr/drivers/gpio.h>

// GPIO pin configuration
#define LED0_NODE DT_ALIAS(led0)
#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

// Function prototypes
int gpio_init(void);
void gpio_set_led_state(bool state);

#endif // GPIO_CONTROL_H