#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "gpio_control.h"
#include "scpi_commands.h"
#include "tcp_server.h"
#include "http_server.h"


K_THREAD_STACK_DEFINE(tcp_server_stack, TCP_SERVER_STACK_SIZE);

struct k_thread tcp_server_thread_data;

int main(void) {
    printk("Starting LXI I/O Expander\n");

    // Initialize GPIO
    gpio_init();

    // Initialize SCPI
    scpi_init();

    // Start TCP server in a separate thread
    k_thread_create(&tcp_server_thread_data, tcp_server_stack,
                    K_THREAD_STACK_SIZEOF(tcp_server_stack),
                    (k_thread_entry_t)tcp_server, NULL, NULL, NULL,
                    7, 0, K_NO_WAIT);

    http_server_start();

    // Add a small delay to allow both threads to start
    k_msleep(1);

    return 0;
}
