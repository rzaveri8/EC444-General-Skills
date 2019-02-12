#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include <http_server.h>
#include <nvs_flash.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "rangefinder.h"

#define WIFI_SSID "Group_16"
#define WIFI_PASS "smart-systems"
#define LED (21)

static const char *TAG="APP";
int LED_level = 0;
int distance;

/* An HTTP GET handler */
esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char* test_str = "SAMPLE TEXT";
    httpd_resp_send(req, test_str, strlen(test_str));
    return ESP_OK;
}

esp_err_t ir_handler(httpd_req_t *req)
{
    char char_dist[10];
    sprintf(char_dist, "%d", distance);
    const char* response = char_dist;
    httpd_resp_send(req, response, strlen(response));
    printf("IR REQUEST\n");
    return ESP_OK;
}

esp_err_t toggleon_handler(httpd_req_t *req) {
    LED_level = 1;
    gpio_set_level(LED, 1);
    const char* status = "1";
    httpd_resp_send(req, status, strlen(status));
    printf("LED ON REQUEST\n");
    return ESP_OK;
}

esp_err_t toggleoff_handler(httpd_req_t *req) {
    LED_level = 0;
    gpio_set_level(LED, 0);
    const char* status = "0";
    httpd_resp_send(req, status, strlen(status));
    printf("LED OFF REQUEST\n");
    return ESP_OK;
}

httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    .user_ctx  = NULL
};

httpd_uri_t toggleon = {
  .uri = "/toggleon",
  .method = HTTP_GET,
  .handler = toggleon_handler,
  .user_ctx = NULL
};

httpd_uri_t toggleoff = {
  .uri = "/toggleoff",
  .method = HTTP_GET,
  .handler = toggleoff_handler,
  .user_ctx = NULL
};

httpd_uri_t ir = {
  .uri = "/ir",
  .method = HTTP_GET,
  .handler = ir_handler,
  .user_ctx = NULL
};

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: %d", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &toggleon);
        httpd_register_uri_handler(server, &toggleoff);
        httpd_register_uri_handler(server, &ir);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static esp_err_t event_handler(void *ctx, system_event_t *event) {
    httpd_handle_t *server = (httpd_handle_t *) ctx;

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "Got IP: %s",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        /* Start the web server */
        if (*server == NULL) {
            *server = start_webserver();
        }
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        ESP_ERROR_CHECK(esp_wifi_connect());

        /* Stop the web server */
        if (*server) {
            stop_webserver(*server);
            *server = NULL;
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void init_wifi(void *arg) {
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main() {
    static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    init_wifi(&server);
    init_ir();
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);
    while (1) {
      distance = getDistance_ir();
      vTaskDelay(pdMS_TO_TICKS(500));
    }
}
