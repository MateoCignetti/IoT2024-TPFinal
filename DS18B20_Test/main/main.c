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

//Variables
static const gpio_num_t SENSOR_GPIO = 18;
static const ds18x20_addr_t SENSOR_ADDR = 0xaf000000162f2828;
static const char *TAG = "ds18x20_test";
bool status = 0;

//Handlers
SemaphoreHandle_t xSemaphore = NULL;
httpd_handle_t httpd_server = NULL;

//Funtions
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
    init_webserver();
    float temperature;
    esp_err_t res;

    while (1)
    {
        res = ds18x20_measure_and_read(SENSOR_GPIO, SENSOR_ADDR, &temperature);
        if (res != ESP_OK)
            ESP_LOGE(TAG, "Could not read from sensor %08" PRIx32 "%08" PRIx32 ": %d (%s)",
                    (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, res, esp_err_to_name(res));
        else
            ESP_LOGI(TAG, "Sensor %08" PRIx32 "%08" PRIx32 ": %.2f°C",
                    (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, temperature);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }


}

void app_main()
{
    xTaskCreate(test, TAG, configMINIMAL_STACK_SIZE * 4, NULL, 5, NULL);
}