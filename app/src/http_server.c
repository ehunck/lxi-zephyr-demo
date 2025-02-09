#include "http_server.h"
#include <zephyr/kernel.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/http/parser.h>
#include <stdio.h>
#include <string.h>

// Define the HTML form
static const char html_form[] = 
    "<html><body>"
    "<h1>LXI Device Configuration</h1>"
    "<form method='POST' action='/configure'>"
    "IP Address: <input type='text' name='ip_address'><br>"
    "Subnet Mask: <input type='text' name='subnet_mask'><br>"
    "Gateway: <input type='text' name='gateway'><br>"
    "<input type='submit' value='Configure'>"
    "</form>"
    "</body></html>";

// Define response buffer
static char response_buf[1024];
static char recv_buf[1024];

// HTTP server context
static struct http_parser_settings parser_settings;
static struct http_parser parser;

static void http_response_ok(int sock, const char *payload, size_t payload_len)
{
    char header[256];
    int header_len;

    header_len = snprintf(header, sizeof(header),
                         "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n"
                         "Content-Length: %zu\r\n"
                         "Connection: close\r\n"
                         "\r\n",
                         payload_len);

    zsock_sendto(sock, header, header_len, 0, NULL, 0);
    zsock_sendto(sock, payload, payload_len, 0, NULL, 0);
}

static void process_http_request(int client_sock, char *req_buf, size_t req_len)
{
    if (strstr(req_buf, "GET / ") != NULL) {
        // Handle root GET request
        http_response_ok(client_sock, html_form, strlen(html_form));
    } else if (strstr(req_buf, "POST /configure") != NULL) {
        // Handle configuration POST request
        char *body = strstr(req_buf, "\r\n\r\n");
        if (body) {
            body += 4; // Skip the empty line
            printk("Received configuration: %s\n", body);
            
            const char *response = "Configuration updated successfully!";
            http_response_ok(client_sock, response, strlen(response));
        }
    } else {
        // Handle unknown request
        const char *not_found = "404 Not Found";
        http_response_ok(client_sock, not_found, strlen(not_found));
    }
}

void http_server_thread()
{
    int serv_sock;
    struct sockaddr_in bind_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(80),
        .sin_addr = { .s_addr = INADDR_ANY },
    };

    serv_sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock < 0) {
        printk("Failed to create HTTP server socket (%d)\n", errno);
        return;
    }

    if (zsock_bind(serv_sock, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        printk("Failed to bind HTTP server socket (%d)\n", errno);
        zsock_close(serv_sock);
        return;
    }

    if (zsock_listen(serv_sock, 5) < 0) {
        printk("Failed to listen HTTP server socket (%d)\n", errno);
        zsock_close(serv_sock);
        return;
    }

    printk("HTTP server listening on port 80\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sock;
        ssize_t recv_len;

        client_sock = zsock_accept(serv_sock, (struct sockaddr *)&client_addr,
                                 &client_addr_len);
        if (client_sock < 0) {
            printk("Failed to accept HTTP connection (%d)\n", errno);
            continue;
        }

        recv_len = zsock_recvfrom(client_sock, recv_buf, sizeof(recv_buf) - 1, 
                                 0, NULL, 0);
        if (recv_len > 0) {
            recv_buf[recv_len] = '\0';
            process_http_request(client_sock, recv_buf, recv_len);
        }

        zsock_close(client_sock);
    }
}

