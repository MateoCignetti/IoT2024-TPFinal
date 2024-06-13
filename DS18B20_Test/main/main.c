//CONGIFURE:
// 1. Set the GPIO pin to which the DS18B20 sensor is connected
// 2. Set the address of the DS18B20 sensor
// 3. Set the Wi-Fi SSID and password in "wifi.c"
// 4. Set the MQTT broker URL
// 5. Set the topic to which the temperature will be published

//Includes
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/FreeRTOSConfig.h"
#include "freertos/semphr.h"
#include <esp_log.h>
#include <esp_err.h>

//Includes - 18B20
#include <inttypes.h>
#include <ds18x20.h>

//Includes - Wi-Fi
#include "wifi.h"

//Includes - MQTT
#include "mqtt.h"

//Defines
#define CONFIG_BROKER_URL "mqtt://192.168.43.224:1883"
#define TOPIC "sensor_1_send"

//Variables
static const gpio_num_t SENSOR_GPIO = 17;
static const ds18x20_addr_t SENSOR_ADDR = 0xaf000000162f2828;
static const char *TAG = "DS18X20";
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
            char temp_str[16];
            snprintf(temp_str, sizeof(temp_str), "%.2f, 20", temperature);
            mqtt_publish(TOPIC, temp_str);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

//Main
void app_main()
{
    init_wifi_STA();
    mqtt_app_start(CONFIG_BROKER_URL);
    xTaskCreate(test, TAG, configMINIMAL_STACK_SIZE * 4, NULL, 5, NULL);
}