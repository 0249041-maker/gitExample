/*
 * mqtt5 — ESP-IDF + Adafruit IO (v5)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "esp_random.h"


// =====================  CONFIGURACIÓN  =====================

// --- Wi-Fi (2.4 GHz) ---
//INSERTAR LINEA DE USUARIO DE WIFI
//INSRTAR LINEA DE CONTRASEÑA WIFI

#define MQTT_BROKER_URI "mqtt://io.adafruit.com:1883"
//INSERTAR LINEA DE USUARIO ADAFRUIT
//INSERTAR LINEA DE CONTRASEÑA ADAFRUIT

// --- Topics requeridos por la tarea (prefijo u0249041) ---
#define TOPIC_TEMP    MQTT_USERNAME "/feeds/u0249041.temperature"
#define TOPIC_GPS     MQTT_USERNAME "/feeds/u0249041.gps"
#define TOPIC_STATUS  MQTT_USERNAME "/feeds/u0249041.status"
#define TOPIC_UPTIME  MQTT_USERNAME "/feeds/u0249041.uptime"

// ===========================================================

static const char *TAG = "MQTT_EXAMPLE";

static esp_mqtt_client_handle_t g_client = NULL;

// Puntos GPS simulados (Guadalajara)
static const char *gps_points[] = {
    "20.6736,-103.3440", "20.6748,-103.3525", "20.6761,-103.3612",
    "20.6774,-103.3689", "20.6802,-103.3755", "20.6840,-103.3801"
};

// ---------- Publicaciones periódicas (cada 5 s) ----------
static void publish_task(void *arg)
{
    int uptime = 0;
    size_t gi = 0;

    while (1) {
        // Temperatura simulada 26.5..29.4
        float temp = 26.5f + (esp_random() % 30) / 10.0f;
        char tbuf[16];  snprintf(tbuf, sizeof tbuf, "%.1f", temp);
        esp_mqtt_client_publish(g_client, TOPIC_TEMP, tbuf, 0, 1, 0);
        ESP_LOGI(TAG, "PUB %s -> %s", TOPIC_TEMP, tbuf);

        // GPS (lat,lon)
        const char *gps = gps_points[gi];
        esp_mqtt_client_publish(g_client, TOPIC_GPS, gps, 0, 1, 0);
        ESP_LOGI(TAG, "PUB %s -> %s", TOPIC_GPS, gps);
        gi = (gi + 1) % (sizeof(gps_points)/sizeof(gps_points[0]));

        // Estado / uptime
        esp_mqtt_client_publish(g_client, TOPIC_STATUS, "online", 0, 0, 0);
        char ubuf[16]; snprintf(ubuf, sizeof ubuf, "%d", uptime);
        esp_mqtt_client_publish(g_client, TOPIC_UPTIME, ubuf, 0, 0, 0);
        ESP_LOGI(TAG, "PUB %s -> %s", TOPIC_UPTIME, ubuf);
        uptime += 5;

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

// ---------- Manejador de eventos MQTT ----------
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t e = (esp_mqtt_event_handle_t)event_data;

    switch (e->event_id) {

    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT connected");
        // Suscríbete a los cuatro (o al menos a TEMP) si quieres ver eco en monitor
        esp_mqtt_client_subscribe(g_client, TOPIC_TEMP,   0);
        esp_mqtt_client_subscribe(g_client, TOPIC_GPS,    0);
        esp_mqtt_client_subscribe(g_client, TOPIC_STATUS, 0);
        esp_mqtt_client_subscribe(g_client, TOPIC_UPTIME, 0);
        // Arranca publicaciones periódicas
        xTaskCreate(publish_task, "pub_task", 4096, NULL, 5, NULL);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        // Imprime usando longitudes para evitar basura
        printf("Topic: %.*s\n", e->topic_len, e->topic);
        printf("Msg  : %.*s\n", e->data_len,  e->data);
        break;

    default:
        ESP_LOGI(TAG, "Other MQTT event id:%d", e->event_id);
        break;
    }
}

// ---------- Inicializa y arranca el cliente MQTT ----------
static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri        = MQTT_BROKER_URI,
        .session.protocol_ver      = MQTT_PROTOCOL_V_3_1_1,   // 👈 usa 3.1.1 para Adafruit IO
        .credentials.username      = MQTT_USERNAME,
        .credentials.authentication.password = MQTT_PASSWORD,
    };

    g_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(g_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(g_client);
}


// ---------- Wi-Fi ----------
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Retrying Wi-Fi connection...");

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Wi-Fi connected!");
        mqtt_app_start();
    }
}

static void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *netif = esp_netif_create_default_wifi_sta(); (void)netif;

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t any_id, got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                        &wifi_event_handler, NULL, &any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                        &wifi_event_handler, NULL, &got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// ---------- app_main ----------
void app_main(void)
{
    // NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();
}
