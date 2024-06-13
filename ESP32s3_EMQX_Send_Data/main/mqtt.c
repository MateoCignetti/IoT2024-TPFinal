//Includes
#include "mqtt.h"
#include "esp_log.h"

//Variables
static const char *TAG = "MQTT";

//Handlers
esp_mqtt_client_handle_t mqtt_client = NULL; // Cliente MQTT

//Function definitions

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
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
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(const char *broker_url)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = broker_url,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg); // Inicializar cliente MQTT
    // The last argument may be used to pass data to the event handler, in this example mqtt_event_handler
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void mqtt_publish(const char *topic, const char *message)
{
    esp_mqtt_client_publish(mqtt_client, topic, message, 0, 1, 0);
}
