//Includes --------------------------------------------------------------------------------------------------------------
#include "wifi.h"

//Defines ---------------------------------------------------------------------------------------------------------------
#define AP_SSID     "P18-APtoSTA"   //AP Wi-Fi network SSID
#define AP_PASSWORD "holamundo"     //AP Wi-Fi network password

//Private function prototypes -------------------------------------------------------------------------------------------
static void wifi_lwip_init();       //Initialize LightweightIP network stack (TCP/IP protocol) and Wi-Fi driver
static void wifi_config_AP();       //Configure Wi-Fi connection in AP mode with specified SSID and password
static void wifi_start();           //Start Wi-Fi connection in AP mode

//Functions -------------------------------------------------------------------------------------------------------------

//Configure and initialize Wi-Fi as AP mode
void setup_wifi_AP(){
    wifi_lwip_init();
    wifi_config_AP();
    wifi_start();
}

//Initialize LightweightIP network stack (TCP/IP protocol) and Wi-Fi driver
static void wifi_lwip_init(){
    nvs_flash_init();                       //Initialize non-volatile flash memory (NVS) for configuration storage
    esp_netif_init();                       //Initialize ESP-IDF network interface
    esp_event_loop_create_default();        //Create a default event loop to handle network events #NOT DEVELOPED
    esp_netif_create_default_wifi_ap();     //Create a Wi-Fi interface in access point (AP) mode
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT(); //Default Wi-Fi configuration
    wifi_config.nvs_enable = 0;             //Disable NVS usage for Wi-Fi configuration
    esp_wifi_init(&wifi_config);            //Initialize Wi-Fi driver with default configuration
}

//Configure Wi-Fi connection in AP mode with specified SSID and password
static void wifi_config_AP(){
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,                //Set Wi-Fi network SSID
            .password = AP_PASSWORD,        //Set Wi-Fi network password
            .ssid_len = strlen(AP_SSID),    //Calculate SSID length
            .authmode = WIFI_AUTH_WPA2_PSK, //Set authentication mode (WPA2-PSK)
            .max_connection = 5,            //Set maximum number of allowed clients
        }
    };
    esp_wifi_set_mode(WIFI_MODE_AP);        //Set Wi-Fi mode to access point (AP) mode
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);  //Set AP mode parameters with the provided configuration
}

//Configure Wi-Fi connection in STA mode with specified SSID and password
void wifi_config_STA(const char *STA_SSID, const char *STA_PASSWORD){
    esp_netif_create_default_wifi_sta();    //Create a Wi-Fi interface in station (STA) mode
    wifi_config_t wifi_config_station;      //Define Wi-Fi configuration structure

    //Copy the SSID and password to the Wi-Fi configuration structure
    strcpy((char *)wifi_config_station.sta.ssid, STA_SSID);
    strcpy((char *)wifi_config_station.sta.password, STA_PASSWORD);

    printf("SSID: %s\t", wifi_config_station.sta.ssid);           //For DEBUG
    printf("Password: %s\n", wifi_config_station.sta.password);   //For DEBUG

    esp_wifi_set_mode(WIFI_MODE_STA);                        //Set Wi-Fi mode to station (STA) mode
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config_station);  //Set STA mode parameters with the provided configuration
    esp_wifi_connect();
}

static void wifi_start(){
    esp_wifi_start();       //Start the Wi-Fi connection
}
