#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "protocol_examples_common.h"
#include "caesar.h"

#define TAG "ESP_TCP_SERVER"
#define LISTEN_PORT 3333

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());  // usa SSID/PASS de menuconfig

    int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(LISTEN_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(listen_sock, 1);
    ESP_LOGI(TAG, "Listening on port %d ...", LISTEN_PORT);

    while (1) {
        struct sockaddr_in6 source_addr; socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr*)&source_addr, &addr_len);
        if (sock < 0) { ESP_LOGE(TAG, "accept failed"); continue; }
        ESP_LOGI(TAG, "Client connected");

        uint8_t rx[128];
        int len = recv(sock, rx, sizeof(rx), 0);
        if (len > 0) {
            char plain[128];
            caesar_decrypt_bytes(rx, (size_t)len, plain, sizeof(plain));
            ESP_LOGI(TAG, "Received %d bytes. Decrypted: %s", len, plain);
            send(sock, rx, len, 0);  // eco
        }

        shutdown(sock, 0);
        close(sock);
        ESP_LOGI(TAG, "Client disconnected");
    }
}
