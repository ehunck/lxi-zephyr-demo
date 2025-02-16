#include "gpio_control.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


static const struct gpio_dt_spec gpios[] = {
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_0), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_1), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_2), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_3), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_4), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_5), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_6), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_7), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_8), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_9), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_10), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_11), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_12), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_13), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_14), gpios),
    GPIO_DT_SPEC_GET(DT_PATH(gpio_nodes, led_15), gpios),
};

int gpio_init(void) 
{
    int ret;
    for (int i = 0; i < 16; i++) {
        if (!device_is_ready(gpios[i].port)) {
            printk("Error: GPIO %d not ready\n", i);
            return -1;
        }

        ret = gpio_pin_configure_dt(&gpios[i], GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
            printk("Error configuring GPIO %d\n", i);
            return -1;
        }
    }
    return 0;
}

void gpio_set_state(int index, bool state) 
{
    if( index < 16 )
    {
        gpio_pin_set(gpios[index].port, gpios[index].pin, state);
    }
}
