#ifndef MQTT_H
#define MQTT_H

//Includes
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

//Public variables
extern esp_mqtt_client_handle_t mqtt_client;

//Public function prototypes
void mqtt_app_start(const char *broker_url);
void mqtt_publish(const char *topic, const char *message);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

#endif // MQTT_H
