/*
 * Copyright (c) 2020 Friedt Professional Engineering Services, Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <zephyr/net/dns_sd.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/socket.h>
#include <zephyr/posix/netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/net/net_event.h>
#include <zephyr/net/net_if.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mdns_echo_service, LOG_LEVEL_DBG);

#include "service.h"

/* A default port of 0 causes Zephyr to assign an ephemeral port */
#define DEFAULT_PORT 0

void service(void);

/* Function to send a welcome message to the client */
static int welcome(struct net_context *ctx)
{
    static const char msg[] = "Bonjour, Zephyr world!\n";
    return net_context_send(ctx, msg, sizeof(msg), NULL, K_NO_WAIT, NULL);
}

/* Function to wait for the network to be ready */
static void wait_for_network(void)
{
    struct net_if *iface = net_if_get_default();

    LOG_INF("Waiting for network to be ready...");
    while (!net_if_is_up(iface)) {
        k_msleep(100);
    }
    LOG_INF("Network is ready");
}

/* Thread entry point for the mDNS service */
void service_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    /* Wait for the network to be ready */
    wait_for_network();

    /* Start the mDNS service */
    service();
}

// Define the callback function
void my_accept_callback(struct net_context *new_context, struct sockaddr *addr, socklen_t addrlen, int status, void *user_data) {
    // Handle the new connection here
    if (status == 0) {
        // Connection accepted, do something with the new_context
    } else {
        // Handle error
    }
}

/* mDNS service implementation */
void service(void)
{
    int r;
    struct net_context *server_ctx;
    struct net_context *client_ctx;
    struct sockaddr server_addr;
    struct sockaddr client_addr;
    socklen_t client_addr_len;
    char addrstr[INET6_ADDRSTRLEN];
    uint8_t line[64];

#if DEFAULT_PORT == 0
    /* The advanced use case: ephemeral port */
#if defined(CONFIG_NET_IPV6)
    DNS_SD_REGISTER_SERVICE(zephyr, CONFIG_NET_HOSTNAME,
                "_zephyr", "_tcp", "local", DNS_SD_EMPTY_TXT,
                &((struct sockaddr_in6 *)&server_addr)->sin6_port);
#elif defined(CONFIG_NET_IPV4)
    DNS_SD_REGISTER_SERVICE(zephyr, CONFIG_NET_HOSTNAME,
                "_zephyr", "_tcp", "local", DNS_SD_EMPTY_TXT,
                &((struct sockaddr_in *)&server_addr)->sin_port);
#endif
#else
    /* The simple use case: fixed port */
    DNS_SD_REGISTER_TCP_SERVICE(zephyr, CONFIG_NET_HOSTNAME,
                    "_zephyr", "local", DNS_SD_EMPTY_TXT, DEFAULT_PORT);
#endif

#if defined(CONFIG_NET_IPV6)
    struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)&server_addr;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_addr = in6addr_any;
    addr6->sin6_port = htons(DEFAULT_PORT); // Set the port after initializing the struct
    DNS_SD_REGISTER_SERVICE(zephyr, CONFIG_NET_HOSTNAME,
                "_zephyr", "_tcp", "local", DNS_SD_EMPTY_TXT,
                addr6->sin6_port);
#elif defined(CONFIG_NET_IPV4)
    struct sockaddr_in *addr4 = (struct sockaddr_in *)&server_addr;
    addr4->sin_family = AF_INET;
    addr4->sin_addr.s_addr = htonl(INADDR_ANY);
    addr4->sin_port = htons(DEFAULT_PORT);
    DNS_SD_REGISTER_SERVICE(zephyr, CONFIG_NET_HOSTNAME,
                "_zephyr", "_tcp", "local", DNS_SD_EMPTY_TXT,
                addr4->sin_port);
#endif


    r = net_context_get(AF_INET6, SOCK_STREAM, IPPROTO_TCP, &server_ctx);
    if (r < 0) {
        LOG_ERR("Failed to get net_context (%d)", r);
        return;
    }

    r = net_context_bind(server_ctx, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (r < 0) {
        LOG_ERR("Failed to bind net_context (%d)", r);
        net_context_unref(server_ctx);
        return;
    }

    LOG_INF("Server listening on port %d", ntohs(((struct sockaddr_in *)&server_addr)->sin_port));

    r = net_context_listen(server_ctx, 1);
    if (r < 0) {
        LOG_ERR("Failed to listen on net_context (%d)", r);
        net_context_unref(server_ctx);
        return;
    }

    for (;;) {
        client_addr_len = sizeof(client_addr);
        r = net_context_accept(server_ctx, my_accept_callback, K_FOREVER, NULL);
        if (r < 0) {
            LOG_ERR("Failed to accept connection (%d)", r);
            continue;
        }

        /* Convert client address to string */
        void *addrp = IS_ENABLED(CONFIG_NET_IPV6)
                          ? (void *)&((struct sockaddr_in6 *)&client_addr)->sin6_addr
                          : (void *)&((struct sockaddr_in *)&client_addr)->sin_addr;
        inet_ntop(client_addr.sa_family, addrp, addrstr, sizeof(addrstr));
        uint16_t client_port = ntohs(IS_ENABLED(CONFIG_NET_IPV6)
                                         ? ((struct sockaddr_in6 *)&client_addr)->sin6_port
                                         : ((struct sockaddr_in *)&client_addr)->sin_port);

        LOG_INF("Accepted connection from [%s]:%u", addrstr, client_port);

        /* Send a banner */
        r = welcome(client_ctx);
        if (r < 0) {
            LOG_ERR("Failed to send welcome message (%d)", r);
            net_context_unref(client_ctx);
            continue;
        }

        /* Echo received data */
        while (true) {
            r = net_context_recv(client_ctx, NULL, K_FOREVER, NULL);
            if (r < 0) {
                LOG_ERR("Failed to receive data (%d)", r);
                break;
            }

            r = net_context_send(client_ctx, line, r, NULL, K_NO_WAIT, NULL);
            if (r < 0) {
                LOG_ERR("Failed to send data (%d)", r);
                break;
            }
        }

        net_context_unref(client_ctx);
    }
}
