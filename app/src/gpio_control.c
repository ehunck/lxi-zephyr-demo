#include "gpio_control.h"

int gpio_init(void) {
    if (!device_is_ready(led.port)) {
        printk("Error: LED device is not ready\n");
        return -1;
    }
    gpio_pin_configure(led.port, led.pin, GPIO_OUTPUT);
    return 0;
}

void gpio_set_led_state(bool state) {
    gpio_pin_set(led.port, led.pin, state);
}
