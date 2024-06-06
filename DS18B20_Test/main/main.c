//Includes
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/FreeRTOSConfig.h"
#include "freertos/semphr.h"

//Includes - 18B20
#include <inttypes.h>
#include <ds18x20.h>
#include <esp_log.h>
#include <esp_err.h>

//Includes - ServerHTTP/Wi-Fi
#include "wifi.h"
#include "webserver.h"

//Includes - MQTT
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "mqtt_client.h"

//Variables
static const gpio_num_t SENSOR_GPIO = 18;
static const ds18x20_addr_t SENSOR_ADDR = 0xaf000000162f2828;
static const char *TAG = "ds18x20_test";
bool status = 0;

//Handlers
SemaphoreHandle_t xSemaphore = NULL;
httpd_handle_t httpd_server = NULL;
/*CONECTAR A BROKER MQTT----------------------------------------------------------------------------------------------------
esp_mqtt_client_handle_t mqtt_client = NULL; // Cliente MQTT
#define CONFIG_BROKER_URL "mqtt://192.168.43.197:1883"

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

//------------------------
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
//--------------------
static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
    };
    
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg); // Inicializar cliente MQTT
    //The last argument may be used to pass data to the event handler, in this example mqtt_event_handler
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}*/
//-----------------------------------------------------------------------------------------------------------------------

//Function to init Web Server and Configure Wi-Fi
void init_webserver(void)
{
    xSemaphore = xSemaphoreCreateBinary();  //Create a binary semaphore
    if(xSemaphore == NULL)
       {
        printf("Error initializing semaphore\n");
        while(true);
       }
    setup_wifi_AP();    //Set up Wi-Fi in Access Point mode
    httpd_server = start_webserver();  //Start the web server

    while(status == 0){
        if(xSemaphoreTake( xSemaphore , portMAX_DELAY ) == pdTRUE){
            vTaskDelay(pdMS_TO_TICKS(1000));
            stop_webserver(httpd_server);               //Stops HTTP server
            vTaskDelay(pdMS_TO_TICKS(2000));
            wifi_config_STA(STA_SSID, STA_PASSWORD);    // Configure Wi-Fi in Station mode
            status = 1;
        }
    }
}

//Main
void test(void *pvParameter)
{
    float temperature;
    esp_err_t res;

    while (1)
    {
        res = ds18x20_measure_and_read(SENSOR_GPIO, SENSOR_ADDR, &temperature);
        if (res != ESP_OK)
            ESP_LOGE(TAG, "Could not read from sensor %08" PRIx32 "%08" PRIx32 ": %d (%s)",
                    (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, res, esp_err_to_name(res));
        else
        {
            ESP_LOGI(TAG, "Sensor %08" PRIx32 "%08" PRIx32 ": %.2f°C",
                    (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, temperature);
            // Convert temperature to string and publish to MQTT
            //char temp_str[16];
            //snprintf(temp_str, sizeof(temp_str), "%.2f", temperature);
            //esp_mqtt_client_publish(mqtt_client, "/topic/temperature", temp_str, 0, 1, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void app_main()
{
    init_webserver();
    //mqtt_app_start();
    xTaskCreate(test, TAG, configMINIMAL_STACK_SIZE * 4, NULL, 5, NULL);
}