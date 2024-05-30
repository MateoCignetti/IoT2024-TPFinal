//Includes --------------------------------------------------------------------------------------------------------------
#include "webserver.h"

//Global variables ------------------------------------------------------------------------------------------------------
char STA_SSID[50];       //Wi-Fi network SSID for STA mode
char STA_PASSWORD[50];   //Wi-Fi network password for STA mode
extern SemaphoreHandle_t xSemaphore;

//Private function prototypes -------------------------------------------------------------------------------------------
static esp_err_t root_handler(httpd_req_t *¨req);       //Handler for GET requests to root URI
static esp_err_t data_post_handler(httpd_req_t *¨req);  //Handler for POST requests to "/post_data" URI
static void stringHandling(char *buf);                  //Function to extract SSID and Password

//URI handlers ----------------------------------------------------------------------------------------------------------
static const httpd_uri_t root_uri = {
    .uri = "/",                        //Root URI (Uniform Resource Identifier) path
    .method = HTTP_GET,                //HTTP GET method
    .handler = root_handler,           //Handler function for root URI GET requests
};
static const httpd_uri_t post_data = {
    .uri = "/post_data",               //URI path for handling data POST requests
    .method = HTTP_POST,               //HTTP POST method
    .handler = data_post_handler,      //Handler function for data POST requests
};

//Functions ------------------------------------------------------------------------------------------------------------
httpd_handle_t start_webserver(){
    httpd_handle_t httpd_server = NULL;
    httpd_config_t httpd_config = HTTPD_DEFAULT_CONFIG();      //Default HTTP server configuration
    httpd_config.stack_size = 2*4096;                          //Double the default stack size for server tasks

    if(httpd_start(&httpd_server, &httpd_config) == ESP_OK){
        httpd_register_uri_handler(httpd_server, &root_uri);   //Register handler for root URI
        httpd_register_uri_handler(httpd_server, &post_data);  //Register handler for post_data URI
        return httpd_server;                                   //Return HTTP server handle
    } else{
        return NULL;
    }
}

void stop_webserver(httpd_handle_t httpd__server)
{
     if (httpd__server != NULL) {
         httpd_stop(httpd__server);
     }
}

//Handles GET requests to the root URI
static esp_err_t root_handler(httpd_req_t *req){
    extern unsigned char view_start[] asm("_binary_view_html_start");   //Access binary data of HTML template
    extern unsigned char view_end[] asm("_binary_view_html_end");       //Access end of binary data of HTML template
    const size_t view_size = (view_end - view_start);                   //Calculate HTML template size
    char viewHtml[view_size];
    memcpy(viewHtml, view_start, view_size);                            //Copy HTML template to a buffer
    httpd_resp_send(req, (char*) viewHtml, view_size);                  //Send the current HTML template as response
    return ESP_OK;
}

//Handles POST requests to "/post_data" URI
esp_err_t data_post_handler(httpd_req_t *req)
{
    char buf[1024];                           //Buffer to store POST data
    int ret, remaining = req->content_len;    //Remaining bytes to read
    
    //Read POST data sent from the form
    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, buf, sizeof(buf))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                // Handle timeout error
                httpd_resp_send_408(req);
            }
            return ESP_FAIL;
        }

        //Process received POST data
        buf[ret] = '\0';        // Null-terminate the received data

        //printf("Received POST data: %s\n", buf);  //DEBUG

        stringHandling(buf);    //Extract SSID and Password from POST data

        //printf("SSID: %s\t", STA_SSID);   //DEBUG
        //printf("Password: %s\n", STA_PASSWORD);   //DEBUG

        remaining -= ret;
    }

    //Sets the HTTP response status code to "303 See Other", indicating that the client should make another request using a new URL.
    httpd_resp_set_status(req, "303 See Other");
    //Sets the "Location" header in the HTTP response, specifing that the client should be redirected to root (URI "/")
    httpd_resp_set_hdr(req, "Location", "/");
    //Sends the HTTP response to the client.
    httpd_resp_send(req, NULL, 0);
    xSemaphoreGive(xSemaphore);     //Release semaphore to indicate that the Wi-Fi configuration has been received, and the device should switch to STA mode
    return ESP_OK;                  //Return status indicating successful processing of POST request
}

//Function to extract SSID and Password
static void stringHandling(char *buf) {
    char *n_ptr = strstr(buf, "n="); //Find position of "n=" substring
    char *p_ptr = strstr(buf, "&p="); //Find position of "&p=" substring

    n_ptr += strlen("n=");           //Move pointer after "n="
    size_t len = p_ptr - n_ptr;      //Get lenght of substring between "n=" and "&p="
    strncpy(STA_SSID, n_ptr, len);   //Copy substring between "n=" and "&p="
    //STA_SSID[len] = '\0';          //Null character to end string

    p_ptr += strlen("&p=");          //Move pointer after "&p="
    strcpy(STA_PASSWORD, p_ptr);     //Copy string after "&p="
}