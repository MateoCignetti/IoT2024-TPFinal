#ifndef WEBSERVER_H
#define WEBSERVER_H

//Includes --------------------------------------------------------------------------------------------------------------
#include <string.h>
#include "esp_http_server.h"
#include "freertos/semphr.h"

//Global variables ------------------------------------------------------------------------------------------------------
extern char STA_SSID[50];       //Wi-Fi network SSID for STA mode
extern char STA_PASSWORD[50];   //Wi-Fi network password for STA mode
//extern httpd_handle_t httpd_server;

//Public function prototypes --------------------------------------------------------------------------------------------
httpd_handle_t start_webserver();
void stop_webserver(httpd_handle_t httpd__server);

#endif