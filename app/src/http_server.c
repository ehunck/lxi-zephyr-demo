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

// static struct http_resource_detail_static main_js_gz_resource_detail = {
// 	.common = {
// 			.type = HTTP_RESOURCE_TYPE_STATIC,
// 			.bitmask_of_supported_http_methods = BIT(HTTP_GET),
// 			.content_encoding = "none",
// 			.content_type = "text/html",
// 		},
// 	.static_data = main_js_gz,
// 	.static_data_len = sizeof(main_js_gz),
// };

static uint16_t http_service_port = 80;

HTTP_SERVICE_DEFINE(test_http_service, "0.0.0.0", &http_service_port, 1, 10, NULL);

HTTP_RESOURCE_DEFINE(index_html_gz_resource, test_http_service, "/",
		     &index_html_gz_resource_detail);

// HTTP_RESOURCE_DEFINE(main_js_gz_resource, test_http_service, "/configure",
// 		     &main_js_gz_resource_detail);


