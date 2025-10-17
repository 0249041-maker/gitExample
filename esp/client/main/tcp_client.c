#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "esp_log.h"
#include "caesar.h"

#define TAG "ESP_TCP_CLIENT"

// CAMBIA esta IP si tu PC cambia de red:
#define HOST_IP_ADDR "10.70.31.118"
#define PORT 3333
#define SHIFT_VALUE 5

void tcp_client(void)
{
    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port   = htons(PORT);
    inet_pton(AF_INET, HOST_IP_ADDR, &dest_addr.sin_addr);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "socket failed, errno=%d", errno);
        return;
    }

    ESP_LOGI(TAG, "Connecting to %s:%d ...", HOST_IP_ADDR, PORT);
    if (connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) != 0) {
        ESP_LOGE(TAG, "connect failed, errno=%d", errno);
        close(sock);
        return;
    }
    ESP_LOGI(TAG, "Connected!");

    const char *msg = "Carmen_123";
    uint8_t pkt[128];
    size_t n = caesar_encrypt_bytes(msg, SHIFT_VALUE, pkt, sizeof(pkt));
    int sent = send(sock, pkt, n, 0);
    ESP_LOGI(TAG, "Sent %d bytes (shift=%d)", sent, SHIFT_VALUE);

    // Eco opcional y descifrado para mostrar en logs
    uint8_t rx[128];
    int r = recv(sock, rx, sizeof(rx), 0);
    if (r > 0) {
        char plain[128];
        caesar_decrypt_bytes(rx, (size_t)r, plain, sizeof(plain));
        ESP_LOGI(TAG, "Echo %d bytes. Decrypted: %s", r, plain);
    }

    shutdown(sock, 0);
    close(sock);
}
