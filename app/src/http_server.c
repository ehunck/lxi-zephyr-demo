#include "http_server.h"
#include <stdio.h>
#include <inttypes.h>
#include <zephyr/kernel.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include "zephyr/device.h"
#include "zephyr/sys/util.h"
#include <zephyr/drivers/led.h>
#include <zephyr/data/json.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(http_server, LOG_LEVEL_DBG);

static const uint8_t index_html_gz[] = {
    #include "index.html.gz.inc"
};

static struct http_resource_detail_static index_html_gz_resource_detail = {
	.common = {
			.type = HTTP_RESOURCE_TYPE_STATIC,
			.bitmask_of_supported_http_methods = BIT(HTTP_GET),
			.content_encoding = "gzip",
			.content_type = "text/html",
		},
	.static_data = index_html_gz,
	.static_data_len = sizeof(index_html_gz),
};

static const uint8_t millipede_logo_html_gz[] = {
    #include "millipede-logo-dark-gray.png.gz.inc"
};

static struct http_resource_detail_static millipede_logo_html_gz_resource_detail = {
	.common = {
			.type = HTTP_RESOURCE_TYPE_STATIC,
			.bitmask_of_supported_http_methods = BIT(HTTP_GET),
			.content_encoding = "gzip",
			.content_type = "text/html",
		},
	.static_data = millipede_logo_html_gz,
	.static_data_len = sizeof(millipede_logo_html_gz),
};


int apply_network_config(const char* ip_address, const char* subnet_mask, const char* gateway)
{
	// Apply the network configuration
	// TODO: Implement this function
	return 0;
}

// Define the handler for the "/configure" HTTP resource
static int dyn_configure_handler(struct http_client_ctx *client,
                                 enum http_data_status status,
                                 const struct http_request_ctx *request_ctx,
                                 struct http_response_ctx *response_ctx,
                                 void *user_data)
{
   #define MAX_TEMP_PRINT_LEN 32
    static char print_str[MAX_TEMP_PRINT_LEN];
    enum http_method method = client->method;
    static size_t processed;

    __ASSERT_NO_MSG(buffer != NULL);

    if (status == HTTP_SERVER_DATA_ABORTED) {
        LOG_DBG("Transaction aborted after %zd bytes.", processed);
        processed = 0;
        return 0;
    }

    processed += request_ctx->data_len;

    snprintf(print_str, sizeof(print_str), "%s received (%zd bytes)",
             http_method_str(method), request_ctx->data_len);
    LOG_HEXDUMP_DBG(request_ctx->data, request_ctx->data_len, print_str);

    if (status == HTTP_SERVER_DATA_FINAL) {
        LOG_DBG("All data received (%zd bytes).", processed);
        processed = 0;
    }

	char ip_address[16] = {0};
	char subnet_mask[16] = {0};
	char gateway[16] = {0};

	/* Parse the data */
	int count = sscanf(request_ctx->data, "ip_address=%s&subnet_mask=%s&gateway=%s", ip_address, subnet_mask, gateway);	
	if (count == 3) {
		int success = apply_network_config(ip_address, subnet_mask, gateway);
		if (success == 0) {
			LOG_INF("Network configuration applied successfully.");
		} else {
			LOG_ERR("Failed to apply network configuration.");
		}
	}

    /* Echo data back to client */
    response_ctx->body = request_ctx->data;
    response_ctx->body_len = request_ctx->data_len;
    response_ctx->final_chunk = (status == HTTP_SERVER_DATA_FINAL);

    return 0;
}


struct http_resource_detail_dynamic configure_resource_detail = {
    .common = {
        .type = HTTP_RESOURCE_TYPE_DYNAMIC,
        .bitmask_of_supported_http_methods = BIT(HTTP_POST),
    },
    .cb = dyn_configure_handler,
    .user_data = NULL,
};

static uint16_t http_service_port = 80;

HTTP_SERVICE_DEFINE(test_http_service, "0.0.0.0", &http_service_port, 1, 10, NULL);

HTTP_RESOURCE_DEFINE(index_html_gz_resource, test_http_service, "/",
		     &index_html_gz_resource_detail);

HTTP_RESOURCE_DEFINE(millipede_logo_html_gz_resource, test_http_service, "/millipede-logo-dark-gray.png",
		     &millipede_logo_html_gz_resource_detail);

HTTP_RESOURCE_DEFINE(configure_resource, test_http_service, "/configure",
                     &configure_resource_detail);
