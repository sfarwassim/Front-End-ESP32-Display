/* LVGL Example project
 * 
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "nvs_flash.h"

#include <sys/param.h>
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "cJSON.h"
#include "PuraLib.h"
#include "nvs.h"
#include "esp_timer.h"
#include "esp_sleep.h"

/* Littlevgl specific */
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "qm_ui_entry.h"

/* blockly specific*/
/*********************
 *      DEFINES
 *********************/
#define TAG "main"
#define LV_TICK_PERIOD_MS 10
#define EXAMPLE_ESP_WIFI_SSID "Electriclabs"
#define EXAMPLE_ESP_WIFI_PASS "elabs2019"
#define EXAMPLE_ESP_MAXIMUM_RETRY 5
#define TOUCH_TYPE 112
#define MINI_TYPE 113
#define SHUTTER_TYPE 114
#define MINI4R_TYPE 116
#define SHUTTER4R_TYPE 117
#define TOUCH_CHANNELS 16
#define MINI_CHANNELS 9
#define MINI4R_CHANNELS 11

char rx_buffer10[2000];
char TempoZone[50];
char rx_buffer[2000];
char rx_udp_buffer[1460];
elm_data_t dataDevice;
char selectedDropDownZone[64];
char Temperature[3];
char rx_buffer40[2000];

extern int TypeRequest;
nvs_handle_t my_handle;
nvs_handle_t my_handle2;
nvs_handle_t my_handle3;

int TypeChannel;
#define HOST_IP_ADDR "192.168.1.89"

#define PORT 10555
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void
lv_tick_task(void *arg);
void guiTask(void *pvParameter);
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

/**********************
 *   APPLICATION MAIN
 **********************/
// UDP receive
static void elm_deserialize(const char *buffer, elm_data_t *data)
{
    static char tbuf[500];
    memcpy(tbuf, buffer, sizeof(tbuf));

    char *token = strtok(tbuf, "*");

    // memset(data, 0, sizeof(elm_data_t));

    int iter = 1;
    while (token != NULL && iter < 8)
    {
        switch (iter)
        {
        case 1:
            data->blk1 = (short)atoi(token);
            break;
        case 2:
            data->blk2 = (short)atoi(token);
            break;
        case 3:
            data->blk3 = (short)atoi(token);
            break;
        case 4:
            data->blk4 = (short)atoi(token);
            break;
        case 5:
            snprintf(data->message, sizeof(data->message), "%s\n", token);
            break;
        case 6:
            snprintf(data->verify_d, sizeof(data->verify_d), "%s\n", token);
            break;
        case 7:
            snprintf(data->verify_e, sizeof(data->verify_e), "%s\n", token);
            break;

        default:
            break;
        }
        iter++;
        token = strtok(NULL, "*");
    }
}
static void elm_parse(const char *elm_buf) //int src as parameter
{
    int state;
    elm_deserialize(elm_buf, &dataDevice);
    char mac_sta_cstr[20];
    uint8_t mac[6];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    snprintf(mac_sta_cstr, sizeof(mac_sta_cstr), "%02x:%02x:%02x:%02x:%02x:%02x", MAC2STR(mac));
    //Load the channels in our flash memory
    esp_err_t err = nvs_open("nvs", NVS_READWRITE, &my_handle3);
    if (err == ESP_OK)
    {
        size_t required_size = 0;

        err = nvs_get_str(my_handle3, "DataCollected", NULL, &required_size);
        err = nvs_get_str(my_handle3, "DataCollected", rx_buffer, &required_size);

        switch (err)
        {
        case ESP_OK:
            printf("Done for channels for udp check \n");

            cJSON *root = cJSON_Parse(rx_buffer);
            cJSON *channells;
            int TypeChannel = cJSON_GetObjectItem(root, "type")->valueint;
            channells = cJSON_GetObjectItem(root, "channels");
            int channelsSize = cJSON_GetArraySize(channells);
            for (int i = 0; i < channelsSize; i++)
            {

                cJSON *arrayChannel = cJSON_GetArrayItem(channells, i);
                int number = cJSON_GetObjectItem(arrayChannel, "number")->valueint;
                char *macaddress = cJSON_GetObjectItem(arrayChannel, "macaddress")->valuestring;
                state = cJSON_GetObjectItem(arrayChannel, "state")->valueint;
                char *name = cJSON_GetObjectItem(arrayChannel, "name")->valuestring;
                if (dataDevice.blk1 == TOUCH_TYPE || dataDevice.blk1 == MINI_TYPE || dataDevice.blk1 == SHUTTER_TYPE || dataDevice.blk1 == MINI4R_TYPE || dataDevice.blk1 == SHUTTER4R_TYPE)
                {
                    //if (strncmp(dataDevice.verify_d, mac_sta_cstr, sizeof(dataDevice.verify_d)) != 0)
                    if (strncmp(dataDevice.verify_d, macaddress, sizeof(dataDevice.verify_d)) == 0)
                    {

                        // This state is not mine
                        uint8_t *remote_channels;
                        uint8_t channels_number = 0;
                        if (dataDevice.blk1 == TOUCH_TYPE)
                        {
                            // Touch
                            remote_channels = (uint8_t *)malloc(sizeof(uint8_t) * TOUCH_CHANNELS);
                            channels_number = TOUCH_CHANNELS;
                            printf("TOUCH_TYPE\n");
                        }
                        else if (dataDevice.blk1 == MINI_TYPE || dataDevice.blk1 == SHUTTER_TYPE)
                        {
                            // Mini / Shutter have the same number of channels
                            remote_channels = (uint8_t *)malloc(sizeof(uint8_t) * MINI_CHANNELS);
                            channels_number = MINI_CHANNELS;
                            printf("MINI_TYPE\n");
                        }
                        else if (dataDevice.blk1 == MINI4R_TYPE || dataDevice.blk1 == SHUTTER4R_TYPE)
                        {
                            // Mini4R / Shutter4R have the same number of channels
                            remote_channels = (uint8_t *)malloc(sizeof(uint8_t) * MINI4R_CHANNELS);
                            channels_number = MINI4R_CHANNELS;
                            printf("MINI4R_TYPE\n");
                        }
                        else
                        {
                            // This point should not be reachable. EVER. otherwise it means memory corruption
                            return;
                        }

                        for (int i = 0; i < channels_number; ++i)
                        {
                            remote_channels[i] = (uint8_t)dataDevice.message[i] - 48;
                        }

                        if (remote_channels[number] != state)
                        {
                            // cJSON_AddNumberToObject(arrayChannel, "state", remote_channels[number]);
                            state = remote_channels[number];
                            printf("state updated\n");
                        }
                        update(name, remote_channels[number]);
                    }
                }
            }
            cJSON_Delete(root);

            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");

            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(err));
        }
    }

    nvs_close(my_handle3);
    // elm_data_t elm_data;

    // printf("This is my device Mac %s", mac_sta_cstr);
}

////
static void udp_receive_task(void *pvParameters)
{
    // char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    while (1)
    {

        //receive
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(9555);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0)
        {
            printf("Unable to create socket Udp: errno %d\n", errno);
            break;
        }
        // printf("Socket created, sending to %s:%d", HOST_IP_ADDR, PORT);

        int errr = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (errr < 0)
        {
            printf("Socket unable to bind: errno %d\n", errno);
        }
        printf("Socket bound, port %d\n", 9555);

        while (1)
        {
            socklen_t socklen = sizeof(dest_addr);
            int len = recvfrom(sock, rx_udp_buffer, sizeof(rx_udp_buffer) - 1, 0, (struct sockaddr *)&dest_addr, &socklen);
            printf("Waiting for data\n");

            // Error occurred during receiving
            if (len < 0)
            {
                printf("recvfrom failed: errno %d\n", errno);
                break;
            }
            // Data received
            else
            {

                rx_udp_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                printf("Received %d bytes\n ", len);
                printf("%s\n", rx_udp_buffer);

                if (len < 100)
                {
                    elm_parse(rx_udp_buffer);
                    // printf("Mac address is : %s\n", dataDevice.verify_d);
                }
                if (strncmp(rx_udp_buffer, "OK:\n ", 4) == 0)
                {
                    printf("Received expected message, reconnecting\n");
                    break;
                }
            }

            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }

        if (sock != -1)
        {
            printf("Shutting down socket and restarting...\n");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

////Flash Memory Nvs
void save_config_nvs()
{
    printf("You are saving data now Request 20\n");
    esp_err_t err = nvs_open("nvs", NVS_READWRITE, &my_handle);
    err = nvs_set_str(my_handle, "DataCollected", rx_buffer);
    err = nvs_commit(my_handle);
    nvs_close(my_handle);
}

//WIFI STATION CODE

static int s_retry_num = 0;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}
static void user_nvs_init()
{
    esp_err_t ret;
    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

//The tcp
static void tcp_client_task(void *pvParameters)
{
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;
    printf("The TypeRequest in main : %d\n", TypeRequest);
    unsigned long last_time = 0;

    while (1)
    {
        if (esp_timer_get_time() - last_time > 30000000)
        {
            TypeRequest = 30;
            last_time = esp_timer_get_time();
        }

        if (TypeRequest != NULL)
        {
            struct sockaddr_in dest_addr;
            dest_addr.sin_addr.s_addr = inet_addr(host_ip);
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(PORT);

            addr_family = AF_INET;
            ip_protocol = IPPROTO_IP;

            int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
            if (sock < 0)
            {
                printf("Unable to create socket: errno\n");
                break;
            }
            printf("Socket created, connecting to %s:%d\n", host_ip, PORT);

            int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
            if (err != 0)
            {
                printf("Socket unable to connect: errno %d\n", errno);
                printf("Socket unable to connect\n");
                break;
            }
            printf("Successfully connected\n");

            while (1)
            {
                // load_selected_zone();
                //Load zone from the flash memory
                esp_err_t errr = nvs_open("nvs2", NVS_READWRITE, &my_handle2);
                if (errr == ESP_OK)
                {
                    size_t required_size = 0;

                    errr = nvs_get_str(my_handle2, "zoneName", NULL, &required_size);
                    required_size = sizeof(selectedDropDownZone);
                    errr = nvs_get_str(my_handle2, "zoneName", selectedDropDownZone, &required_size);

                    switch (errr)
                    {
                    case ESP_OK:
                        printf("Done for the selected zone\n");
                        // printf("Downloaded Data: %s\n", selectedDropDownZone);

                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");

                        break;
                    default:
                        printf("Error (%s) reading!\n", esp_err_to_name(errr));
                    }
                }
                nvs_close(my_handle2);

                cJSON *root2;
                root2 = cJSON_CreateObject();
                printf(" Type request %d\n", TypeRequest);
                switch (TypeRequest)
                {
                case 10:
                    /* code */
                    cJSON_AddNumberToObject(root2, "Type", TypeRequest);
                    cJSON_AddStringToObject(root2, "Zone", "");

                    break;

                case 20:
                    /* code */

                    cJSON_AddNumberToObject(root2, "Type", TypeRequest);
                    cJSON_AddStringToObject(root2, "Zone", selectedDropDownZone);

                    break;
                case 30:
                    /* code */
                    cJSON_AddNumberToObject(root2, "Type", TypeRequest);
                    cJSON_AddStringToObject(root2, "Zone", selectedDropDownZone);

                    break;
                case 40:
                    /* code */
                    cJSON_AddNumberToObject(root2, "Type", TypeRequest);

                    break;
                default:
                    // cJSON_AddStringToObject(root2, "not", "nothing");

                    break;
                }
                const char *my_json_string = cJSON_Print(root2);
                int err = send(sock, my_json_string, strlen(my_json_string), 0);

                cJSON_Delete(root2);

                if (err < 0)
                {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }

                if (TypeRequest == 10)
                {
                    int len10 = recv(sock, rx_buffer10, sizeof(rx_buffer10) - 1, 0);
                    printf("RX BUUFER 10 : %s\n", rx_buffer10);
                }
                else if (TypeRequest == 20)
                {
                    int len20 = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
                    printf("RX BUFFER 20: %s\n", rx_buffer);
                    save_config_nvs();
                    rx_buffer[len20] = 0; // Null-terminate whatever we received and treat like a string
                }
                else if (TypeRequest == 30)
                {
                    int len30 = recv(sock, TempoZone, sizeof(TempoZone) - 1, 0);
                    printf("Tempo is : %s\n", TempoZone);
                    cJSON *root3 = cJSON_Parse(TempoZone);
                    int temperature = cJSON_GetObjectItem(root3, "temperature")->valueint;
                    int TypeZone = cJSON_GetObjectItem(root3, "type")->valueint;
                    cJSON_Delete(root3);
                    snprintf(Temperature, sizeof(Temperature), "%d", temperature);

                    printf("Temperature is %d ", temperature);
                }
                else if (TypeRequest == 40)
                {
                    int len40 = recv(sock, rx_buffer40, sizeof(rx_buffer40) - 1, 0);
                    printf("RX BUFFER 40: %s\n", rx_buffer40);
                    save_config_nvs();
                    rx_buffer40[len40] = 0; // Null-terminate whatever we received and treat like a string
                }
                else
                {
                    printf(" There is no request\n");
                }
                TypeRequest = 0;

                vTaskDelay(2000 / portTICK_PERIOD_MS);
            }

            if (sock != -1)
            {
                ESP_LOGE(TAG, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void blocklyTask(void *pvParameter)
{
    lvgl_blockly_init();
    while (1)
    {
        lvgl_blockly_loop();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    //A task should NEVER return
    vTaskDelete(NULL);
}

void deviceTask(void *pvParameter)
{
    lvgl_device_init();
    while (1)
    {
        lvgl_device_loop();
        // vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void app_main()
{

    user_nvs_init();
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
    //If you want to use a task to create the graphic, you NEED to create a Pinned task
    //Otherwise there can be problem such as memory corruption and so on
    xTaskCreatePinnedToCore(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(udp_receive_task, "udp_receive_task", 4096, NULL, 5, NULL, 1);

    xTaskCreatePinnedToCore(deviceTask, "device", 4096 * 2, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(blocklyTask, "blockly", 4096 * 2, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
}

static void lv_tick_task(void *arg)
{
    (void)arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}

extern void lv_8ms_init(void);

//Creates a semaphore to handle concurrent call to lvgl stuff
//If you wish to call *any* lvgl function from other threads/tasks
//you should lock on the very same semaphore!
SemaphoreHandle_t xGuiSemaphore;

void guiTask(void *pvParameter)
{

    (void)pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    static lv_color_t buf1[DISP_BUF_SIZE];
#ifndef CONFIG_LVGL_TFT_DISPLAY_MONOCHROME
    static lv_color_t buf2[DISP_BUF_SIZE];
#endif
    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_IL3820
    /* Actual size in pixel, not bytes and use single buffer */
    size_in_px *= 8;
    lv_disp_buf_init(&disp_buf, buf1, NULL, size_in_px);
#elif defined CONFIG_LVGL_TFT_DISPLAY_MONOCHROME
    lv_disp_buf_init(&disp_buf, buf1, NULL, size_in_px);
#else
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);
#endif

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

#ifdef CONFIG_LVGL_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

#if CONFIG_LVGL_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    lv_8ms_init();

    lv_qm_ui_entry();
    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        //Try to lock the semaphore, if success, call lvgl stuff
        if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE)
        {
            lv_qm_ui_loop();
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
    //A task should NEVER return
    vTaskDelete(NULL);
}
