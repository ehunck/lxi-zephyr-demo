#include "tcp_server.h"
#include <zephyr/net/socket.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/kernel.h>
#include <string.h>
#include "scpi_commands.h"

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>


void tcp_server(void) {
    int sock, client;
    struct sockaddr_in addr;
    char buffer[256];
    int ret;
    sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        printk("Failed to create socket\n");
        return;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (zsock_bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printk("Failed to bind socket\n");
        zsock_close(sock);
        return;
    }

    if (zsock_listen(sock, 1) < 0) {
        printk("Failed to listen on socket\n");
        zsock_close(sock);
        return;
    }

    printk("TCP server listening on port %d\n", TCP_SERVER_PORT);
    while (1) {
        client = zsock_accept(sock, NULL, NULL);
        if (client < 0) {
            printk("Failed to accept connection\n");
            continue;
        }
        // Set the client socket as the user context for SCPI
        scpi_context.user_context = &client;
        while (1) {
            ret = zsock_recv(client, buffer, sizeof(buffer), 0);
            if (ret <= 0) {
                break;
            }
            // Pass the received command to libscpi
            SCPI_Input(&scpi_context, buffer, ret);
            // // Send response back to the client
            // const char *response = SCPI_Result(&scpi_context);
            // send(client, response, strlen(response), 0);
        }
        zsock_close(client);
    }
}
