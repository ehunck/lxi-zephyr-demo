#include "scpi_commands.h"
#include "gpio_control.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/net/socket.h>

// Define the SCPI context
scpi_t scpi_context;

// Define the SCPI command table
static const scpi_command_t scpi_commands[] = {
    { .pattern = "DIGital:OUTPut#", .callback = SCPI_DigitalOutputQ },
    { .pattern = "ANALog:INPut#", .callback = SCPI_AnalogInputQ },
    SCPI_CMD_LIST_END
};

// Write callback: SEnd data back to the client
size_t scpi_write(scpi_t *context, const char *data, size_t len) {
    int client = *(int*)context->user_context; // Get the client socket from the context
    return zsock_send(client, data, len, 0); // Send data to the client
}

// Error callback: Hanlde SCPI errors
void scpi_error(scpi_t *context, scpi_error_t error) {
    printk("SCPI Error: %d\n", error);
}

// Define the SCPI interface
static scpi_interface_t scpi_interface = {
    .write = scpi_write,
    .error = scpi_error,
    .reset = NULL,
};

// Initialize the SCPI context
void scpi_init(void) {
    SCPI_Init(&scpi_context, scpi_commands, &scpi_interface, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0);
}

// SCPI command handler for digital output query
scpi_result_t SCPI_DigitalOutputQ(scpi_t *context) {
    int32_t state;
    if(!SCPI_ParamInt(context, &state, true)) {
        return SCPI_RES_ERR;
    }

    gpio_set_led_state(state);
    return SCPI_RES_OK;
}

// SCPI command handler for analog input query
scpi_result_t SCPI_AnalogInputQ(scpi_t *context) {
    int32_t channel;
    if(!SCPI_ParamInt(context, &channel, true)) {
        return SCPI_RES_ERR;
    }

    // Read analog input value
    // int32_t value = read_analog_input(channel);
    SCPI_ResultInt(context, 1234);
    return SCPI_RES_OK;
}

