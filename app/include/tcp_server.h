#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <zephyr/kernel.h>

// Define the TCP server port
#define TCP_SERVER_STACK_SIZE 2048
#define TCP_SERVER_PORT 5025

// Function prototype for the TCP server
void tcp_server(void);

#endif // TCP_SERVER_H