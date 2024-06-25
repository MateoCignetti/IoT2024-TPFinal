//CONGIFURE:
// 1. Set the GPIO pin to which the DHT11 sensor is connected
// 2. Set the Wi-Fi SSID and password in "wifi.c"
// 3. Set the MQTT broker URL
// 4. Set the topic to which the temperature will be published

//Includes
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/FreeRTOSConfig.h"
#include "freertos/semphr.h"
#include <esp_log.h>
#include <esp_err.h>

//Includes - DHT
#include <dht.h>

//Includes - Wi-Fi
#include "wifi.h"

//Includes - MQTT
#include "mqtt.h"

//Defines
#define CONFIG_BROKER_URL "mqtt://192.168.43.224:1883"
#define TOPIC "sensor_1_send"
#define SENSOR_TYPE DHT_TYPE_DHT11
#define DELAY_MEASUREMENTS 3000

//Variables
static const gpio_num_t SENSOR_GPIO = 17;
static const char *TAG = "DHT11";
bool status = 0;

//Function to init Wi-Fi station mode
void init_wifi_STA(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}

//Main Task
void test(void *pvParameter)
{
    float temperature, humidity;
    esp_err_t res;

    while (1)
    {
        res = dht_read_float_data(SENSOR_TYPE, SENSOR_GPIO, &humidity, &temperature);
        if (res != ESP_OK)
            ESP_LOGE(TAG, "Could not read from sensor: %d (%s)", res, esp_err_to_name(res));
        else
        {
            ESP_LOGI(TAG, "Temperature: %.0f°C, Humidity: %.0f%%", temperature, humidity);
            char temp_str[16];
            snprintf(temp_str, sizeof(temp_str), "%.0f, %.0f", temperature, humidity);
            mqtt_publish(TOPIC, temp_str);
        }
        vTaskDelay(pdMS_TO_TICKS(DELAY_MEASUREMENTS));
    }
}

//Main
void app_main()
{
    init_wifi_STA();
    mqtt_app_start(CONFIG_BROKER_URL);
    xTaskCreate(test, TAG, configMINIMAL_STACK_SIZE * 4, NULL, 5, NULL);
}
