/***
* author:wassim
* platform: esp
* version: 1.0.0
* UIResolution: 480*320
*/

#include "qm_ui_entry.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lwip/sockets.h"
#include "PuraLib.h"
#include "nvs.h"
#include "cJSON.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

char udpMsg[1400] = {'\0'};
char udpMsg2[1400] = {'\0'};

char selectedDropDownZone[64];
int TypeRequest;
char listOfZones[2000];
char rx_buffer[2000];
char rx_buffer10[2000];
extern char rx_buffer40[2000];

extern char Temperature[3];
int state;

nvs_handle_t my_handle;
nvs_handle_t my_handle2;

lv_obj_t *list_of_actions_btn = NULL;
lv_obj_t *list_action_2_btn = NULL;

void lvgl_blockly_loop();
void lvgl_blockly_init();
/*
    * includes
    */
LV_IMG_DECLARE(img1623332624197_png);
LV_IMG_DECLARE(img1621853429446_png);
LV_IMG_DECLARE(img1621853423805_png);
LV_IMG_DECLARE(img1620211926196_png);
LV_IMG_DECLARE(img1620212521459_png);
/*
    * declear main_screen
    */
lv_obj_t *main_screen;
lv_obj_t *labelTempoReadAverage;
lv_obj_t *slider;
lv_obj_t *btn0;
lv_obj_t *labelTempoCompter;
lv_obj_t *labelTempoSet;
lv_obj_t *ButtonSetTempo;
lv_obj_t *configBtn;
/*
    * declear screenConfig1
    */
lv_obj_t *screenConfig1;
lv_obj_t *label_5f0b;
lv_obj_t *label_cbd8;
lv_obj_t *btnForwardLoadZonesConfig1;
/*
    * declear screenConfig2
    */
lv_obj_t *screenConfig2;
lv_obj_t *label_9580;
lv_obj_t *btnForwardConfig2;
lv_obj_t *ListZonesdropdown;
/*
    * declear screenConfig3
    */
lv_obj_t *screenConfig3;
lv_obj_t *label_63c0;
lv_obj_t *label_3f53;
lv_obj_t *btnForwardConfig3;
/*
    * declear screenAction1
    */
lv_obj_t *screenAction1;
lv_obj_t *btnActionBack1;
static lv_obj_t *list_of_actions;
lv_obj_t *btnActionnext;

/*
    * declear screenAction2
    */
lv_obj_t *screenAction2;
lv_obj_t *list_action_2;
lv_obj_t *btnActionnext2;

LV_FONT_DECLARE(ali_font_16);
LV_FONT_DECLARE(ali_font_34);
LV_FONT_DECLARE(ali_font_67);
LV_FONT_DECLARE(ali_font_19);
LV_FONT_DECLARE(ali_font_20);
LV_FONT_DECLARE(ali_font_39);
LV_FONT_DECLARE(ali_font_41);
LV_FONT_DECLARE(ali_font_46);
LV_FONT_DECLARE(ali_font_47);

void show_screen_main_screen();
void show_screen_screenConfig1();
void show_screen_screenConfig2();
void show_screen_screenConfig3();
void show_screen_screenAction1();
void show_screen_screenAction2();

/*
    * keyboard
    */
lv_obj_t *g_kb = NULL;

static void g_kb_event_cb(lv_obj_t *event_kb, lv_event_t event)
{
    /* Just call the regular event handler */
    if (event == LV_EVENT_APPLY)
    {
        lv_obj_set_hidden(event_kb, true);
    }
    else if (event == LV_EVENT_CANCEL)
    {
        lv_obj_set_hidden(event_kb, true);
    }
    else
    {
        lv_keyboard_def_event_cb(event_kb, event);
    }
}
static void g_create_kb_ifneeded()
{
    if (g_kb == NULL)
    {
        g_kb = lv_keyboard_create(lv_scr_act(), NULL);
        lv_obj_set_pos(g_kb, 5, 90);
        lv_obj_set_event_cb(g_kb, g_kb_event_cb); /* Setting a custom event handler stops the keyboard from closing automatically */
        lv_obj_set_size(g_kb, LV_HOR_RES - 10, 140);
        lv_keyboard_set_cursor_manage(g_kb, true); /* Automatically show/hide cursors on text areas */
        lv_obj_set_hidden(g_kb, true);
    }
}
static void g_show_kb()
{
    g_create_kb_ifneeded();
    lv_obj_set_parent(g_kb, lv_scr_act());
    lv_obj_set_hidden(g_kb, false);
    lv_obj_align(g_kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

/**
    *开机启动屏相关
    */
static lv_style_t style_star, style_black, style_url;
LV_IMG_DECLARE(img_8ms_png);
LV_IMG_DECLARE(img_star);
lv_obj_t *logo;
lv_obj_t *logo_star;
lv_obj_t *url_label;
lv_obj_t *url_mask;
lv_obj_t *mask_layer;
int timer_cnt = 0;
/****
    * 临时变量用于数据传输
    */
char _tmpbuf[32];

extern void update(char *name, int state)
{
    lv_obj_t *child = lv_list_get_next_btn(list_of_actions, NULL);
    while (child)
    {
        /*Do something with "child" */
        if (strncmp(lv_list_get_btn_text(child), name, sizeof(lv_list_get_btn_text(child))) == 0)
        {
            printf("name is : %s", name);
            if (state == 1)
            {
                lv_obj_set_style_local_bg_color(child, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
                // lv_obj_set_style_local_bg_color(child, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x50, 0xe3, 0xc2));

                // printf("Child is : %s\n", lv_list_get_btn_text(child));
            }
            else if (state == 0)
            {
                lv_obj_set_style_local_bg_color(child, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x50, 0xe3, 0xc2));
                printf("Child is : %s\n", lv_list_get_btn_text(child));
            }
        }

        child = lv_list_get_next_btn(list_of_actions, child);
    }
}

/*
    * declear codes
    */
void elm_serialize(int b1, int b2, int b3, int b4, const char *msg, const char *vd, const char *ve, char *elm_string)
{
    memset(elm_string, '\0', 1400);
    snprintf(elm_string, 1400, "%d*%d*%d*%d*%s*%s*%s", b1, b2, b3, b4, msg, vd, ve);
}
#define BLOCKLY_init

static void blockly_init()
{
    lv_slider_set_value(slider, 37, LV_ANIM_OFF);
}

#define BLOCKLY_loop

static void blockly_loop()
{

    if (Temperature != NULL)
    {
        lv_label_set_text(labelTempoReadAverage, Temperature);
    }
}

#define BLOCKLY_configBtn_EVENT_HANDLER

static void configBtn_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_SHORT_CLICKED)
    {

        TypeRequest = 10;

        show_screen_screenConfig1();
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
    }
}

#define BLOCKLY_btnForwardLoadZonesConfig1_EVENT_HANDLER

static void btnForwardLoadZonesConfig1_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_SHORT_CLICKED)
    {
        // TypeRequest = 10;
        lv_dropdown_clear_options(ListZonesdropdown);

        cJSON *root2 = cJSON_Parse(rx_buffer10);

        cJSON *zones;

        int TypeZone = cJSON_GetObjectItem(root2, "type")->valueint;
        zones = cJSON_GetObjectItem(root2, "Zones");
        int zonesSize = cJSON_GetArraySize(zones);
        char *zone;
        for (int i = 0; i < zonesSize; i++)
        {
            cJSON *arrayZone = cJSON_GetArrayItem(zones, i);
            zone = cJSON_GetObjectItem(arrayZone, "GroupeLevel")->valuestring;
            strcat(listOfZones, zone);
            strcat(listOfZones, "\n");
        }
        cJSON_Delete(root2);
        // printf("here wassim %s\n", listOfZones);
        if (listOfZones != NULL)
        {
            lv_dropdown_set_options(ListZonesdropdown,
                                    listOfZones);
        }

        show_screen_screenConfig2();
    }

    else if (event == LV_EVENT_LONG_PRESSED)
    {
    }
}

#define BLOCKLY_btnForwardConfig2_EVENT_HANDLER

static void btnForwardConfig2_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_SHORT_CLICKED)
    {

        lv_dropdown_get_selected_str(ListZonesdropdown, selectedDropDownZone, sizeof selectedDropDownZone);
        esp_err_t err = nvs_open("nvs2", NVS_READWRITE, &my_handle2);
        if (selectedDropDownZone != NULL)
        {
            err = nvs_set_str(my_handle2, "zoneName", selectedDropDownZone);
            err = nvs_commit(my_handle2);
            nvs_close(my_handle2);
        }

        show_screen_screenConfig3();
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
    }
}
#define BLOCKLY_list_of_actions_EVENT_HANDLER

static void list_of_actions_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        esp_err_t err = nvs_open("nvs", NVS_READWRITE, &my_handle);
        if (err == ESP_OK)
        {
            size_t required_size = 0;

            err = nvs_get_str(my_handle, "DataCollected", NULL, &required_size);
            err = nvs_get_str(my_handle, "DataCollected", rx_buffer, &required_size);

            switch (err)
            {
            case ESP_OK:
                printf("Done for channels\n");

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
                    int state = cJSON_GetObjectItem(arrayChannel, "state")->valueint;
                    char *name = cJSON_GetObjectItem(arrayChannel, "name")->valuestring;
                    if (strcmp(name, lv_list_get_btn_text(obj)) == 0)
                    {
                        elm_serialize(55, 1, number, 0, "0", macaddress, "ERA", udpMsg);
                        int addr_family = 0;
                        int ip_protocol = 0;

                        //Sending @
                        struct sockaddr_in send_addr;
                        send_addr.sin_addr.s_addr = inet_addr("192.168.1.255");
                        send_addr.sin_family = AF_INET;
                        send_addr.sin_port = htons(9555);
                        addr_family = AF_INET;
                        ip_protocol = IPPROTO_IP;

                        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
                        if (sock < 0)
                        {
                            printf("Unable to create socket\n");
                        }
                        printf("Socket created, sending \n");

                        int errr = sendto(sock, udpMsg, strlen(udpMsg), 0, (struct sockaddr *)&send_addr, sizeof(send_addr));
                        if (errr < 0)
                        {
                            printf("Error occurred during sending\n");
                        }
                        printf("Message sent\n");
                        memset(udpMsg, 0, sizeof(udpMsg));
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
        nvs_close(my_handle);
        /////////////////////////////////////////////
    }
}
#define BLOCKLY_list_of_actions2_EVENT_HANDLER

static void list_of_actions2_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {

        printf("clicked %s\n", lv_list_get_btn_text(obj));
        cJSON *root5 = cJSON_Parse(rx_buffer40);
        cJSON *channells;
        int TypeChannel = cJSON_GetObjectItem(root5, "type")->valueint;
        channells = cJSON_GetObjectItem(root5, "channels");
        int channelsSize = cJSON_GetArraySize(channells);
        for (int i = 0; i < channelsSize; i++)
        {

            cJSON *arrayChannel = cJSON_GetArrayItem(channells, i);
            int number = cJSON_GetObjectItem(arrayChannel, "number")->valueint;
            char *macaddress = cJSON_GetObjectItem(arrayChannel, "macaddress")->valuestring;
            int state = cJSON_GetObjectItem(arrayChannel, "state")->valueint;
            char *name = cJSON_GetObjectItem(arrayChannel, "name")->valuestring;
            if (strcmp(name, lv_list_get_btn_text(obj)) == 0)
            {
                printf("name is %s\n", name);

                elm_serialize(55, 3, number, 0, "0", macaddress, "ERA", udpMsg2);
                int addr_family = 0;
                int ip_protocol = 0;

                //Sending @
                struct sockaddr_in send_addr;
                send_addr.sin_addr.s_addr = inet_addr("192.168.1.255");
                send_addr.sin_family = AF_INET;
                send_addr.sin_port = htons(9555);
                addr_family = AF_INET;
                ip_protocol = IPPROTO_IP;

                int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
                if (sock < 0)
                {
                    printf("Unable to create socket\n");
                }
                printf("Socket created, sending \n");

                int errr = sendto(sock, udpMsg2, strlen(udpMsg2), 0, (struct sockaddr *)&send_addr, sizeof(send_addr));
                if (errr < 0)
                {
                    printf("Error occurred during sending\n");
                }
                printf("Message sent\n");
                memset(udpMsg2, 0, sizeof(udpMsg2));
            }
        }

        cJSON_Delete(root5);
    }
}

#define BLOCKLY_slider_EVENT_HANDLER

static void slider_event_handler(lv_obj_t *obj, lv_event_t event)
{
    static char nub[10];

    if (event == LV_EVENT_VALUE_CHANGED)
    {
        int a = lv_slider_get_value(slider);
        // ////////////////////////
        snprintf(nub, sizeof(nub), "%d", a);
        lv_label_set_text(labelTempoSet, nub);
    }
}
#define BLOCKLY_btnForwardConfig3_EVENT_HANDLER

static void btnForwardConfig3_event_handler(lv_obj_t *obj, lv_event_t event)
{

    if (event == LV_EVENT_SHORT_CLICKED)
    {
        show_screen_main_screen();
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
    }
}

#define BLOCKLY_btn0_EVENT_HANDLER

static void btn0_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_SHORT_CLICKED)
    {
        lv_list_clean(list_of_actions);
        TypeRequest = 20;
        ///Update buttons
        esp_err_t err = nvs_open("nvs", NVS_READWRITE, &my_handle);
        if (err == ESP_OK)
        {
            size_t required_size = 0;

            err = nvs_get_str(my_handle, "DataCollected", NULL, &required_size);
            err = nvs_get_str(my_handle, "DataCollected", rx_buffer, &required_size);

            switch (err)
            {
            case ESP_OK:
                printf("Done for channels\n");
                // printf("The TypeRequest: %d\n", TypeRequest);

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
                    int state = cJSON_GetObjectItem(arrayChannel, "state")->valueint;
                    char *name = cJSON_GetObjectItem(arrayChannel, "name")->valuestring;
                    LV_IMG_DECLARE(img1620211926196_png);
                    list_of_actions_btn = lv_list_add_btn(list_of_actions, &img1620211926196_png, name);
                    lv_obj_set_parent(list_of_actions_btn, list_of_actions);
                    lv_obj_set_style_local_text_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
                    lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
                    lv_obj_set_style_local_border_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x8a, 0x97));
                    lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
                    lv_obj_set_event_cb(list_of_actions_btn, list_of_actions_event_handler);

                    if (state == 1)
                    {
                        lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
                    }
                    else if (state == 0)
                    {
                        lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x50, 0xe3, 0xc2));
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
        nvs_close(my_handle);

        show_screen_screenAction1();
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
    }
}

#define BLOCKLY_btnActionBack1_EVENT_HANDLER

static void btnActionBack1_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_SHORT_CLICKED)
    {
        show_screen_main_screen();
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
    }
}

#define BLOCKLY_btnActionnext_EVENT_HANDLER

static void btnActionnext_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_SHORT_CLICKED)
    {
        TypeRequest = 40;

        lv_list_clean(list_action_2);

        cJSON *root = cJSON_Parse(rx_buffer40);
        cJSON *channells;
        int TypeChannel = cJSON_GetObjectItem(root, "type")->valueint;
        channells = cJSON_GetObjectItem(root, "channels");
        int channelsSize = cJSON_GetArraySize(channells);
        for (int i = 0; i < channelsSize; i++)
        {

            cJSON *arrayChannel = cJSON_GetArrayItem(channells, i);
            int number = cJSON_GetObjectItem(arrayChannel, "number")->valueint;
            char *macaddress = cJSON_GetObjectItem(arrayChannel, "macaddress")->valuestring;
            int state = cJSON_GetObjectItem(arrayChannel, "state")->valueint;
            char *name = cJSON_GetObjectItem(arrayChannel, "name")->valuestring;
            LV_IMG_DECLARE(img1620211926196_png);
            list_action_2_btn = lv_list_add_btn(list_action_2, &img1620211926196_png, name);
            lv_obj_set_style_local_text_color(list_action_2_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
            lv_obj_set_style_local_bg_color(list_action_2_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
            lv_obj_set_style_local_border_color(list_action_2_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x8a, 0x97));
            lv_obj_set_style_local_bg_color(list_action_2_btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
            lv_obj_set_event_cb(list_action_2_btn, list_of_actions2_event_handler);

            show_screen_screenAction2();
        }
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
    }
}
#define BLOCKLY_btnActionnext2_EVENT_HANDLER

static void btnActionnext2_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_SHORT_CLICKED)
    {
        show_screen_main_screen();
    }
    else if (event == LV_EVENT_LONG_PRESSED)
    {
    }
}

/*
 * callback functions begins
 */
/*
 * callback functions ends
 */
/*
    * init codes
    */
void init_function()
{
#ifdef BLOCKLY_init
    blockly_init();
#endif
}

void lv_8ms_labelTempoReadAverage_create()
{
    labelTempoReadAverage = lv_label_create(main_screen, NULL);
    lv_label_set_long_mode(labelTempoReadAverage, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(labelTempoReadAverage, 185, 157);
    //set label style
    lv_obj_set_style_local_bg_color(labelTempoReadAverage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_set_style_local_bg_opa(labelTempoReadAverage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_text_color(labelTempoReadAverage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
    lv_obj_set_style_local_text_font(labelTempoReadAverage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ali_font_39);

    lv_label_set_text(labelTempoReadAverage, "27 °");
    lv_label_set_align(labelTempoReadAverage, LV_LABEL_ALIGN_CENTER);
#ifdef BLOCKLY_labelTempoReadAverage_EVENT_HANDLER
    lv_obj_set_event_cb(labelTempoReadAverage, labelTempoReadAverage_event_handler);
#endif
    if (labelTempoReadAverage->coords.x2 - labelTempoReadAverage->coords.x1 < 185)
    {
        int x_offset = (185 - (labelTempoReadAverage->coords.x2 - labelTempoReadAverage->coords.x1)) / 2;
        lv_obj_set_style_local_pad_left(labelTempoReadAverage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
        lv_obj_set_style_local_pad_right(labelTempoReadAverage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
    }
    if (labelTempoReadAverage->coords.y2 - labelTempoReadAverage->coords.y1 < 157)
    {
        int y_offset = (157 - (labelTempoReadAverage->coords.y2 - labelTempoReadAverage->coords.y1)) / 2;
        lv_obj_set_style_local_pad_bottom(labelTempoReadAverage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
        lv_obj_set_style_local_pad_top(labelTempoReadAverage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
    }
    lv_obj_set_pos(labelTempoReadAverage, 0, 0);
}
void lv_8ms_slider_create()
{
    slider = lv_slider_create(main_screen, NULL);
    lv_obj_set_size(slider, 417, 30);
    lv_obj_set_pos(slider, 17, 211);
    lv_slider_set_value(slider, 0, LV_ANIM_OFF);

    static lv_style_t slider_style_bg;
    static lv_style_t slider_style_indic;
    static lv_style_t slider_style_knob;

    lv_style_init(&slider_style_bg);
    lv_style_set_bg_color(&slider_style_bg, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&slider_style_bg, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_style_set_border_color(&slider_style_bg, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_style_set_radius(&slider_style_bg, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

    lv_style_init(&slider_style_indic);
    lv_style_set_bg_color(&slider_style_indic, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&slider_style_indic, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_style_set_radius(&slider_style_indic, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_color(&slider_style_indic, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_style_set_shadow_width(&slider_style_indic, LV_STATE_DEFAULT, 5);

    lv_style_init(&slider_style_knob);
    lv_style_set_bg_color(&slider_style_knob, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
    lv_style_set_bg_grad_color(&slider_style_knob, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
    lv_style_set_radius(&slider_style_knob, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&slider_style_knob, LV_STATE_DEFAULT, LV_OPA_70);

    lv_obj_add_style(slider, LV_SLIDER_PART_BG, &slider_style_bg);
    lv_obj_add_style(slider, LV_SLIDER_PART_INDIC, &slider_style_indic);
    lv_obj_add_style(slider, LV_SLIDER_PART_KNOB, &slider_style_knob);

#ifdef BLOCKLY_slider_EVENT_HANDLER
    lv_obj_set_event_cb(slider, slider_event_handler);
#endif
}
void lv_8ms_btn0_create()
{
    btn0 = lv_imgbtn_create(main_screen, NULL);
    LV_IMG_DECLARE(img1620212521459_png);
    lv_imgbtn_set_src(btn0, LV_BTN_STATE_RELEASED, &img1620212521459_png);
    lv_imgbtn_set_src(btn0, LV_BTN_STATE_PRESSED, &img1620212521459_png);
    lv_imgbtn_set_src(btn0, LV_BTN_STATE_DISABLED, &img1620212521459_png);
    lv_obj_set_size(btn0, 58, 61);
    lv_obj_set_pos(btn0, 408, 241);
#ifdef BLOCKLY_btn0_EVENT_HANDLER
    lv_obj_set_event_cb(btn0, btn0_event_handler);
#endif

    //设置正常样式
    static lv_style_t _sty_btn0;
    lv_style_init(&_sty_btn0);
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btn0, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置正常样式结束

    //设置按下样式
    //color part
    lv_style_set_bg_color(&_sty_btn0, LV_STATE_PRESSED, LV_COLOR_MAKE(0x01, 0xa2, 0xb1));
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btn0, LV_STATE_PRESSED, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置按下样式结束

    //设置禁用样式
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btn0, LV_STATE_DISABLED, LV_COLOR_MAKE(0x88, 0x88, 0x88));
    //image style
    //line style

    lv_obj_add_style(btn0, LV_BTN_PART_MAIN, &_sty_btn0); //选项风格
    //设置禁用样式结束
}
void lv_8ms_labelTempoCompter_create()
{
    labelTempoCompter = lv_label_create(main_screen, NULL);
    lv_label_set_long_mode(labelTempoCompter, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(labelTempoCompter, 100, 40);
    //set label style
    lv_obj_set_style_local_bg_color(labelTempoCompter, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_set_style_local_bg_opa(labelTempoCompter, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_text_color(labelTempoCompter, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
    lv_obj_set_style_local_text_font(labelTempoCompter, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ali_font_47);

    lv_label_set_text(labelTempoCompter, "");
    lv_label_set_align(labelTempoCompter, LV_LABEL_ALIGN_CENTER);
#ifdef BLOCKLY_labelTempoCompter_EVENT_HANDLER
    lv_obj_set_event_cb(labelTempoCompter, labelTempoCompter_event_handler);
#endif
    if (labelTempoCompter->coords.x2 - labelTempoCompter->coords.x1 < 100)
    {
        int x_offset = (100 - (labelTempoCompter->coords.x2 - labelTempoCompter->coords.x1)) / 2;
        lv_obj_set_style_local_pad_left(labelTempoCompter, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
        lv_obj_set_style_local_pad_right(labelTempoCompter, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
    }
    if (labelTempoCompter->coords.y2 - labelTempoCompter->coords.y1 < 40)
    {
        int y_offset = (40 - (labelTempoCompter->coords.y2 - labelTempoCompter->coords.y1)) / 2;
        lv_obj_set_style_local_pad_bottom(labelTempoCompter, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
        lv_obj_set_style_local_pad_top(labelTempoCompter, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
    }
    lv_obj_set_pos(labelTempoCompter, 290, 33);
}
void lv_8ms_labelTempoSet_create()
{
    labelTempoSet = lv_label_create(main_screen, NULL);
    lv_label_set_long_mode(labelTempoSet, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(labelTempoSet, 137, 52);
    //set label style
    lv_obj_set_style_local_bg_color(labelTempoSet, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_set_style_local_bg_opa(labelTempoSet, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_text_color(labelTempoSet, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
    lv_obj_set_style_local_text_font(labelTempoSet, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ali_font_46);

    lv_label_set_text(labelTempoSet, "37");
    lv_label_set_align(labelTempoSet, LV_LABEL_ALIGN_CENTER);
#ifdef BLOCKLY_labelTempoSet_EVENT_HANDLER
    lv_obj_set_event_cb(labelTempoSet, labelTempoSet_event_handler);
#endif
    if (labelTempoSet->coords.x2 - labelTempoSet->coords.x1 < 137)
    {
        int x_offset = (137 - (labelTempoSet->coords.x2 - labelTempoSet->coords.x1)) / 2;
        lv_obj_set_style_local_pad_left(labelTempoSet, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
        lv_obj_set_style_local_pad_right(labelTempoSet, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
    }
    if (labelTempoSet->coords.y2 - labelTempoSet->coords.y1 < 52)
    {
        int y_offset = (52 - (labelTempoSet->coords.y2 - labelTempoSet->coords.y1)) / 2;
        lv_obj_set_style_local_pad_bottom(labelTempoSet, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
        lv_obj_set_style_local_pad_top(labelTempoSet, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
    }
    lv_obj_set_pos(labelTempoSet, 290, 87);
}
void lv_8ms_ButtonSetTempo_create()
{
    ButtonSetTempo = lv_btn_create(main_screen, NULL);
    lv_obj_t *ButtonSetTempo_label;
    ButtonSetTempo_label = lv_label_create(ButtonSetTempo, NULL);
    lv_label_set_text(ButtonSetTempo_label, "SET");
    static lv_style_t style_label_ButtonSetTempo_label;
    lv_style_init(&style_label_ButtonSetTempo_label);

    lv_style_set_text_color(&style_label_ButtonSetTempo_label, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
    //style_label_ButtonSetTempo_label.text.font = &ali_font_34;
    lv_style_set_text_font(&style_label_ButtonSetTempo_label, LV_STATE_DEFAULT, &ali_font_34);
    lv_obj_add_style(ButtonSetTempo_label, LV_LABEL_PART_MAIN, &style_label_ButtonSetTempo_label);
    lv_obj_set_size(ButtonSetTempo, 100, 49);
    lv_obj_set_pos(ButtonSetTempo, 308, 17);
#ifdef BLOCKLY_ButtonSetTempo_EVENT_HANDLER
    lv_obj_set_event_cb(ButtonSetTempo, ButtonSetTempo_event_handler);
#endif

    //设置正常样式
    static lv_style_t _sty_ButtonSetTempo;
    lv_style_init(&_sty_ButtonSetTempo);
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_ButtonSetTempo, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置正常样式结束

    //设置按下样式
    //color part
    lv_style_set_bg_color(&_sty_ButtonSetTempo, LV_STATE_PRESSED, LV_COLOR_MAKE(0x01, 0xa2, 0xb1));
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_ButtonSetTempo, LV_STATE_PRESSED, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置按下样式结束

    //设置禁用样式
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_ButtonSetTempo, LV_STATE_DISABLED, LV_COLOR_MAKE(0x88, 0x88, 0x88));
    //image style
    //line style

    lv_obj_add_style(ButtonSetTempo, LV_BTN_PART_MAIN, &_sty_ButtonSetTempo); //选项风格
    //设置禁用样式结束
}
void lv_8ms_configBtn_create()
{
    configBtn = lv_imgbtn_create(main_screen, NULL);
    LV_IMG_DECLARE(img1621853423805_png);
    lv_imgbtn_set_src(configBtn, LV_BTN_STATE_RELEASED, &img1621853423805_png);
    lv_imgbtn_set_src(configBtn, LV_BTN_STATE_PRESSED, &img1621853423805_png);
    lv_imgbtn_set_src(configBtn, LV_BTN_STATE_DISABLED, &img1621853423805_png);
    lv_obj_set_size(configBtn, 63, 61);
    lv_obj_set_pos(configBtn, 31, 241);
#ifdef BLOCKLY_configBtn_EVENT_HANDLER
    lv_obj_set_event_cb(configBtn, configBtn_event_handler);
#endif

    //设置正常样式
    static lv_style_t _sty_configBtn;
    lv_style_init(&_sty_configBtn);
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_configBtn, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置正常样式结束

    //设置按下样式
    //color part
    lv_style_set_bg_color(&_sty_configBtn, LV_STATE_PRESSED, LV_COLOR_MAKE(0x01, 0xa2, 0xb1));
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_configBtn, LV_STATE_PRESSED, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置按下样式结束

    //设置禁用样式
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_configBtn, LV_STATE_DISABLED, LV_COLOR_MAKE(0x88, 0x88, 0x88));
    //image style
    //line style

    lv_obj_add_style(configBtn, LV_BTN_PART_MAIN, &_sty_configBtn); //选项风格
    //设置禁用样式结束
}
void lv_8ms_label_5f0b_create()
{
    label_5f0b = lv_label_create(screenConfig1, NULL);
    lv_label_set_long_mode(label_5f0b, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(label_5f0b, 275, 60);
    //set label style
    lv_obj_set_style_local_bg_color(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_set_style_local_bg_opa(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_radius(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_border_color(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_obj_set_style_local_border_width(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 6);
    lv_obj_set_style_local_text_color(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
    lv_obj_set_style_local_text_font(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ali_font_41);

    lv_label_set_text(label_5f0b, "WELCOME!");
    lv_label_set_align(label_5f0b, LV_LABEL_ALIGN_CENTER);
#ifdef BLOCKLY_label_5f0b_EVENT_HANDLER
    lv_obj_set_event_cb(label_5f0b, label_5f0b_event_handler);
#endif
    if (label_5f0b->coords.x2 - label_5f0b->coords.x1 < 275)
    {
        int x_offset = (275 - (label_5f0b->coords.x2 - label_5f0b->coords.x1)) / 2;
        lv_obj_set_style_local_pad_left(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
        lv_obj_set_style_local_pad_right(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
    }
    if (label_5f0b->coords.y2 - label_5f0b->coords.y1 < 60)
    {
        int y_offset = (60 - (label_5f0b->coords.y2 - label_5f0b->coords.y1)) / 2;
        lv_obj_set_style_local_pad_bottom(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
        lv_obj_set_style_local_pad_top(label_5f0b, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
    }
    lv_obj_set_pos(label_5f0b, 102, 16);
}
void lv_8ms_label_cbd8_create()
{
    label_cbd8 = lv_label_create(screenConfig1, NULL);
    lv_label_set_long_mode(label_cbd8, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(label_cbd8, 380, 54);
    //set label style
    lv_obj_set_style_local_bg_color(label_cbd8, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_set_style_local_bg_opa(label_cbd8, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_text_color(label_cbd8, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_obj_set_style_local_text_font(label_cbd8, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ali_font_19);

    lv_label_set_text(label_cbd8, "Please press forward to start the configuration");
    lv_label_set_align(label_cbd8, LV_LABEL_ALIGN_CENTER);
#ifdef BLOCKLY_label_cbd8_EVENT_HANDLER
    lv_obj_set_event_cb(label_cbd8, label_cbd8_event_handler);
#endif
    if (label_cbd8->coords.x2 - label_cbd8->coords.x1 < 380)
    {
        int x_offset = (380 - (label_cbd8->coords.x2 - label_cbd8->coords.x1)) / 2;
        lv_obj_set_style_local_pad_left(label_cbd8, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
        lv_obj_set_style_local_pad_right(label_cbd8, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
    }
    if (label_cbd8->coords.y2 - label_cbd8->coords.y1 < 54)
    {
        int y_offset = (54 - (label_cbd8->coords.y2 - label_cbd8->coords.y1)) / 2;
        lv_obj_set_style_local_pad_bottom(label_cbd8, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
        lv_obj_set_style_local_pad_top(label_cbd8, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
    }
    lv_obj_set_pos(label_cbd8, 50, 107);
}
void lv_8ms_btnForwardLoadZonesConfig1_create()
{
    btnForwardLoadZonesConfig1 = lv_imgbtn_create(screenConfig1, NULL);
    LV_IMG_DECLARE(img1620212521459_png);
    lv_imgbtn_set_src(btnForwardLoadZonesConfig1, LV_BTN_STATE_RELEASED, &img1620212521459_png);
    lv_imgbtn_set_src(btnForwardLoadZonesConfig1, LV_BTN_STATE_PRESSED, &img1620212521459_png);
    lv_imgbtn_set_src(btnForwardLoadZonesConfig1, LV_BTN_STATE_DISABLED, &img1620212521459_png);
    lv_obj_set_size(btnForwardLoadZonesConfig1, 69, 67);
    lv_obj_set_pos(btnForwardLoadZonesConfig1, 205, 244);
#ifdef BLOCKLY_btnForwardLoadZonesConfig1_EVENT_HANDLER
    lv_obj_set_event_cb(btnForwardLoadZonesConfig1, btnForwardLoadZonesConfig1_event_handler);
#endif

    //设置正常样式
    static lv_style_t _sty_btnForwardLoadZonesConfig1;
    lv_style_init(&_sty_btnForwardLoadZonesConfig1);
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnForwardLoadZonesConfig1, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置正常样式结束

    //设置按下样式
    //color part
    lv_style_set_bg_color(&_sty_btnForwardLoadZonesConfig1, LV_STATE_PRESSED, LV_COLOR_MAKE(0x01, 0xa2, 0xb1));
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnForwardLoadZonesConfig1, LV_STATE_PRESSED, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置按下样式结束

    //设置禁用样式
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnForwardLoadZonesConfig1, LV_STATE_DISABLED, LV_COLOR_MAKE(0x88, 0x88, 0x88));
    //image style
    //line style

    lv_obj_add_style(btnForwardLoadZonesConfig1, LV_BTN_PART_MAIN, &_sty_btnForwardLoadZonesConfig1); //选项风格
    //设置禁用样式结束
}
void lv_8ms_label_9580_create()
{
    label_9580 = lv_label_create(screenConfig2, NULL);
    lv_label_set_long_mode(label_9580, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(label_9580, 343, 37);
    //set label style
    lv_obj_set_style_local_bg_color(label_9580, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_set_style_local_bg_opa(label_9580, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_text_color(label_9580, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
    lv_obj_set_style_local_text_font(label_9580, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ali_font_20);

    lv_label_set_text(label_9580, "Please Choose your Zone!");
    lv_label_set_align(label_9580, LV_LABEL_ALIGN_CENTER);
#ifdef BLOCKLY_label_9580_EVENT_HANDLER
    lv_obj_set_event_cb(label_9580, label_9580_event_handler);
#endif
    if (label_9580->coords.x2 - label_9580->coords.x1 < 343)
    {
        int x_offset = (343 - (label_9580->coords.x2 - label_9580->coords.x1)) / 2;
        lv_obj_set_style_local_pad_left(label_9580, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
        lv_obj_set_style_local_pad_right(label_9580, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
    }
    if (label_9580->coords.y2 - label_9580->coords.y1 < 37)
    {
        int y_offset = (37 - (label_9580->coords.y2 - label_9580->coords.y1)) / 2;
        lv_obj_set_style_local_pad_bottom(label_9580, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
        lv_obj_set_style_local_pad_top(label_9580, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
    }
    lv_obj_set_pos(label_9580, 60, 12);
}
void lv_8ms_btnForwardConfig2_create()
{
    btnForwardConfig2 = lv_imgbtn_create(screenConfig2, NULL);
    LV_IMG_DECLARE(img1620212521459_png);
    lv_imgbtn_set_src(btnForwardConfig2, LV_BTN_STATE_RELEASED, &img1620212521459_png);
    lv_imgbtn_set_src(btnForwardConfig2, LV_BTN_STATE_PRESSED, &img1620212521459_png);
    lv_imgbtn_set_src(btnForwardConfig2, LV_BTN_STATE_DISABLED, &img1620212521459_png);
    lv_obj_set_size(btnForwardConfig2, 69, 67);
    lv_obj_set_pos(btnForwardConfig2, 403, 247);
#ifdef BLOCKLY_btnForwardConfig2_EVENT_HANDLER
    lv_obj_set_event_cb(btnForwardConfig2, btnForwardConfig2_event_handler);
#endif

    //设置正常样式
    static lv_style_t _sty_btnForwardConfig2;
    lv_style_init(&_sty_btnForwardConfig2);
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnForwardConfig2, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置正常样式结束

    //设置按下样式
    //color part
    lv_style_set_bg_color(&_sty_btnForwardConfig2, LV_STATE_PRESSED, LV_COLOR_MAKE(0x01, 0xa2, 0xb1));
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnForwardConfig2, LV_STATE_PRESSED, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置按下样式结束

    //设置禁用样式
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnForwardConfig2, LV_STATE_DISABLED, LV_COLOR_MAKE(0x88, 0x88, 0x88));
    //image style
    //line style

    lv_obj_add_style(btnForwardConfig2, LV_BTN_PART_MAIN, &_sty_btnForwardConfig2); //选项风格
    //设置禁用样式结束
}
void lv_8ms_ListZonesdropdown_create()
{
    ListZonesdropdown = lv_dropdown_create(screenConfig2, NULL);
    lv_obj_set_size(ListZonesdropdown, 337, 40);
    lv_obj_set_pos(ListZonesdropdown, 81, 68);
    lv_dropdown_set_options(ListZonesdropdown,
                            "Piano primo - cuccina"
                            "\n"
                            "piano primo - sala");
    lv_dropdown_set_draw_arrow(ListZonesdropdown, true);
#ifdef BLOCKLY_ListZonesdropdown_EVENT_HANDLER
    lv_obj_set_event_cb(ListZonesdropdown, ListZonesdropdown_event_handler);
#endif
    /*
设置字体大小颜色
*/
    lv_obj_set_style_local_text_color(ListZonesdropdown, LV_DROPDOWN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_obj_set_style_local_text_color(ListZonesdropdown, LV_DROPDOWN_PART_LIST, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_obj_set_style_local_bg_color(ListZonesdropdown, LV_DROPDOWN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
    lv_obj_set_style_local_bg_color(ListZonesdropdown, LV_DROPDOWN_PART_LIST, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
    /*
设置字体大小颜色结束
*/
}
void lv_8ms_label_63c0_create()
{
    label_63c0 = lv_label_create(screenConfig3, NULL);
    lv_label_set_long_mode(label_63c0, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(label_63c0, 275, 60);
    //set label style
    lv_obj_set_style_local_bg_color(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_set_style_local_bg_opa(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_radius(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_border_color(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_obj_set_style_local_border_width(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 6);
    lv_obj_set_style_local_text_color(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xf5, 0xa6, 0x23));
    lv_obj_set_style_local_text_font(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ali_font_41);

    lv_label_set_text(label_63c0, "GREAT!");
    lv_label_set_align(label_63c0, LV_LABEL_ALIGN_CENTER);
#ifdef BLOCKLY_label_63c0_EVENT_HANDLER
    lv_obj_set_event_cb(label_63c0, label_63c0_event_handler);
#endif
    if (label_63c0->coords.x2 - label_63c0->coords.x1 < 275)
    {
        int x_offset = (275 - (label_63c0->coords.x2 - label_63c0->coords.x1)) / 2;
        lv_obj_set_style_local_pad_left(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
        lv_obj_set_style_local_pad_right(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
    }
    if (label_63c0->coords.y2 - label_63c0->coords.y1 < 60)
    {
        int y_offset = (60 - (label_63c0->coords.y2 - label_63c0->coords.y1)) / 2;
        lv_obj_set_style_local_pad_bottom(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
        lv_obj_set_style_local_pad_top(label_63c0, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
    }
    lv_obj_set_pos(label_63c0, 102, 35);
}
void lv_8ms_label_3f53_create()
{
    label_3f53 = lv_label_create(screenConfig3, NULL);
    lv_label_set_long_mode(label_3f53, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(label_3f53, 380, 54);
    //set label style
    lv_obj_set_style_local_bg_color(label_3f53, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_set_style_local_bg_opa(label_3f53, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 255);
    lv_obj_set_style_local_text_color(label_3f53, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_obj_set_style_local_text_font(label_3f53, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ali_font_19);

    lv_label_set_text(label_3f53, "You can start your JOURNEY now!");
    lv_label_set_align(label_3f53, LV_LABEL_ALIGN_CENTER);
#ifdef BLOCKLY_label_3f53_EVENT_HANDLER
    lv_obj_set_event_cb(label_3f53, label_3f53_event_handler);
#endif
    if (label_3f53->coords.x2 - label_3f53->coords.x1 < 380)
    {
        int x_offset = (380 - (label_3f53->coords.x2 - label_3f53->coords.x1)) / 2;
        lv_obj_set_style_local_pad_left(label_3f53, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
        lv_obj_set_style_local_pad_right(label_3f53, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, x_offset);
    }
    if (label_3f53->coords.y2 - label_3f53->coords.y1 < 54)
    {
        int y_offset = (54 - (label_3f53->coords.y2 - label_3f53->coords.y1)) / 2;
        lv_obj_set_style_local_pad_bottom(label_3f53, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
        lv_obj_set_style_local_pad_top(label_3f53, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, y_offset);
    }
    lv_obj_set_pos(label_3f53, 50, 121);
}
void lv_8ms_btnForwardConfig3_create()
{
    btnForwardConfig3 = lv_imgbtn_create(screenConfig3, NULL);
    LV_IMG_DECLARE(img1620212521459_png);
    lv_imgbtn_set_src(btnForwardConfig3, LV_BTN_STATE_RELEASED, &img1620212521459_png);
    lv_imgbtn_set_src(btnForwardConfig3, LV_BTN_STATE_PRESSED, &img1620212521459_png);
    lv_imgbtn_set_src(btnForwardConfig3, LV_BTN_STATE_DISABLED, &img1620212521459_png);
    lv_obj_set_size(btnForwardConfig3, 69, 67);
    lv_obj_set_pos(btnForwardConfig3, 205, 234);
#ifdef BLOCKLY_btnForwardConfig3_EVENT_HANDLER
    lv_obj_set_event_cb(btnForwardConfig3, btnForwardConfig3_event_handler);
#endif

    //设置正常样式
    static lv_style_t _sty_btnForwardConfig3;
    lv_style_init(&_sty_btnForwardConfig3);
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnForwardConfig3, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置正常样式结束

    //设置按下样式
    //color part
    lv_style_set_bg_color(&_sty_btnForwardConfig3, LV_STATE_PRESSED, LV_COLOR_MAKE(0x01, 0xa2, 0xb1));
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnForwardConfig3, LV_STATE_PRESSED, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置按下样式结束

    //设置禁用样式
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnForwardConfig3, LV_STATE_DISABLED, LV_COLOR_MAKE(0x88, 0x88, 0x88));
    //image style
    //line style

    lv_obj_add_style(btnForwardConfig3, LV_BTN_PART_MAIN, &_sty_btnForwardConfig3); //选项风格
    //设置禁用样式结束
}
void lv_8ms_btnActionBack1_create()
{
    btnActionBack1 = lv_imgbtn_create(screenAction1, NULL);
    LV_IMG_DECLARE(img1621853429446_png);
    lv_imgbtn_set_src(btnActionBack1, LV_BTN_STATE_RELEASED, &img1621853429446_png);
    lv_imgbtn_set_src(btnActionBack1, LV_BTN_STATE_PRESSED, &img1621853429446_png);
    lv_imgbtn_set_src(btnActionBack1, LV_BTN_STATE_DISABLED, &img1621853429446_png);
    lv_obj_set_size(btnActionBack1, 73, 63);
    lv_obj_set_pos(btnActionBack1, 15, 250);
#ifdef BLOCKLY_btnActionBack1_EVENT_HANDLER
    lv_obj_set_event_cb(btnActionBack1, btnActionBack1_event_handler);
#endif

    //设置正常样式
    static lv_style_t _sty_btnActionBack1;
    lv_style_init(&_sty_btnActionBack1);
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnActionBack1, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置正常样式结束

    //设置按下样式
    //color part
    lv_style_set_bg_color(&_sty_btnActionBack1, LV_STATE_PRESSED, LV_COLOR_MAKE(0x01, 0xa2, 0xb1));
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnActionBack1, LV_STATE_PRESSED, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置按下样式结束

    //设置禁用样式
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnActionBack1, LV_STATE_DISABLED, LV_COLOR_MAKE(0x88, 0x88, 0x88));
    //image style
    //line style

    lv_obj_add_style(btnActionBack1, LV_BTN_PART_MAIN, &_sty_btnActionBack1); //选项风格
    //设置禁用样式结束
}
void lv_8ms_btnActionnext_create()
{
    btnActionnext = lv_imgbtn_create(screenAction1, NULL);
    LV_IMG_DECLARE(img1623332624197_png);
    lv_imgbtn_set_src(btnActionnext, LV_BTN_STATE_RELEASED, &img1623332624197_png);
    lv_imgbtn_set_src(btnActionnext, LV_BTN_STATE_PRESSED, &img1623332624197_png);
    lv_imgbtn_set_src(btnActionnext, LV_BTN_STATE_DISABLED, &img1623332624197_png);
    lv_obj_set_size(btnActionnext, 73, 63);
    lv_obj_set_pos(btnActionnext, 376, 250);
#ifdef BLOCKLY_btnActionnext_EVENT_HANDLER
    lv_obj_set_event_cb(btnActionnext, btnActionnext_event_handler);
#endif

    //设置正常样式
    static lv_style_t _sty_btnActionnext;
    lv_style_init(&_sty_btnActionnext);
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnActionnext, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置正常样式结束

    //设置按下样式
    //color part
    lv_style_set_bg_color(&_sty_btnActionnext, LV_STATE_PRESSED, LV_COLOR_MAKE(0x01, 0xa2, 0xb1));
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnActionnext, LV_STATE_PRESSED, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置按下样式结束

    //设置禁用样式
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnActionnext, LV_STATE_DISABLED, LV_COLOR_MAKE(0x88, 0x88, 0x88));
    //image style
    //line style

    lv_obj_add_style(btnActionnext, LV_BTN_PART_MAIN, &_sty_btnActionnext); //选项风格
    //设置禁用样式结束
}
void lv_8ms_btnActionnext2_create()
{
    btnActionnext2 = lv_imgbtn_create(screenAction2, NULL);
    LV_IMG_DECLARE(img1623332624197_png);
    lv_imgbtn_set_src(btnActionnext2, LV_BTN_STATE_RELEASED, &img1623332624197_png);
    lv_imgbtn_set_src(btnActionnext2, LV_BTN_STATE_PRESSED, &img1623332624197_png);
    lv_imgbtn_set_src(btnActionnext2, LV_BTN_STATE_DISABLED, &img1623332624197_png);
    lv_obj_set_size(btnActionnext2, 73, 63);
    lv_obj_set_pos(btnActionnext2, 397, 249);
#ifdef BLOCKLY_btnActionnext2_EVENT_HANDLER
    lv_obj_set_event_cb(btnActionnext2, btnActionnext2_event_handler);
#endif

    //设置正常样式
    static lv_style_t _sty_btnActionnext2;
    lv_style_init(&_sty_btnActionnext2);
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnActionnext2, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置正常样式结束

    //设置按下样式
    //color part
    lv_style_set_bg_color(&_sty_btnActionnext2, LV_STATE_PRESSED, LV_COLOR_MAKE(0x01, 0xa2, 0xb1));
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnActionnext2, LV_STATE_PRESSED, LV_COLOR_MAKE(0x31, 0x40, 0x4f));
    //image style
    //line style

    //设置按下样式结束

    //设置禁用样式
    //color part
    //radius part
    //opa part
    //border part
    //padding part
    //text part
    lv_style_set_text_color(&_sty_btnActionnext2, LV_STATE_DISABLED, LV_COLOR_MAKE(0x88, 0x88, 0x88));
    //image style
    //line style

    lv_obj_add_style(btnActionnext2, LV_BTN_PART_MAIN, &_sty_btnActionnext2); //选项风格
    //设置禁用样式结束
}
void lv_8ms_list_of_actions_create()
{
    list_of_actions = lv_list_create(screenAction1, NULL);
    lv_obj_set_size(list_of_actions, 469, 234);
    lv_obj_set_pos(list_of_actions, 0, 0);

    lv_obj_set_style_local_text_color(list_of_actions, LV_LIST_PART_BG, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_obj_set_style_local_bg_color(list_of_actions, LV_LIST_PART_BG, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
    lv_obj_set_style_local_border_width(list_of_actions, LV_LIST_PART_BG, LV_STATE_DEFAULT, 3);
    lv_obj_set_style_local_border_color(list_of_actions, LV_LIST_PART_BG, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x8a, 0x97));
    lv_obj_set_style_local_bg_color(list_of_actions, LV_LIST_PART_BG, LV_STATE_PRESSED, LV_COLOR_MAKE(0x49, 0x4f, 0x57));

    //     lv_obj_t *list_of_actions_btn;
    //     LV_IMG_DECLARE(img1620211926196_png);
    //     list_of_actions_btn = lv_list_add_btn(list_of_actions, &img1620211926196_png, "button Sala");
    //     lv_obj_set_style_local_text_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    //     lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
    //     lv_obj_set_style_local_border_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x8a, 0x97));
    //     lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_MAKE(0x49, 0x4f, 0x57));
    // #ifdef BLOCKLY_list_of_actions_EVENT_HANDLER
    //     lv_obj_set_event_cb(list_of_actions_btn, list_of_actions_event_handler);
    // #endif
    //     LV_IMG_DECLARE(img1620211926196_png);
    //     list_of_actions_btn = lv_list_add_btn(list_of_actions, &img1620211926196_png, "Button Cuccina");
    //     lv_obj_set_style_local_text_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x49, 0x4f, 0x57));
    //     lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
    //     lv_obj_set_style_local_border_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x8a, 0x97));
    //     lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_MAKE(0x49, 0x4f, 0x57));
    // #ifdef BLOCKLY_list_of_actions_EVENT_HANDLER
    //     lv_obj_set_event_cb(list_of_actions_btn, list_of_actions_event_handler);
    // #endif
    //     LV_IMG_DECLARE(img1620211926196_png);
    //     list_of_actions_btn = lv_list_add_btn(list_of_actions, &img1620211926196_png, "Button Supermercato");
    //     lv_obj_set_style_local_text_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    //     lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
    //     lv_obj_set_style_local_border_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x8a, 0x97));
    //     lv_obj_set_style_local_bg_color(list_of_actions_btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_MAKE(0x49, 0x4f, 0x57));
    // #ifdef BLOCKLY_list_of_actions_EVENT_HANDLER
    //     lv_obj_set_event_cb(list_of_actions_btn, list_of_actions_event_handler);
    // #endif
}
void lv_8ms_list_action_2_create()
{
    list_action_2 = lv_list_create(screenAction2, NULL);
    lv_obj_set_size(list_action_2, 469, 234);
    lv_obj_set_pos(list_action_2, 0, 0);

    lv_obj_set_style_local_text_color(list_action_2, LV_LIST_PART_BG, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xff, 0xff, 0xff));
    lv_obj_set_style_local_bg_color(list_action_2, LV_LIST_PART_BG, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
    lv_obj_set_style_local_border_width(list_action_2, LV_LIST_PART_BG, LV_STATE_DEFAULT, 8);
    lv_obj_set_style_local_border_color(list_action_2, LV_LIST_PART_BG, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x80, 0x8a, 0x97));
    lv_obj_set_style_local_bg_color(list_action_2, LV_LIST_PART_BG, LV_STATE_PRESSED, LV_COLOR_MAKE(0x49, 0x4f, 0x57));
}
void show_screen_main_screen()
{
    lv_scr_load(main_screen);
    //生成动态页面的style
}
void show_screen_screenConfig1()
{
    lv_scr_load(screenConfig1);
    //生成动态页面的style
}
void show_screen_screenConfig2()
{
    lv_scr_load(screenConfig2);
    //生成动态页面的style
}
void show_screen_screenConfig3()
{
    lv_scr_load(screenConfig3);
    //生成动态页面的style
}
void show_screen_screenAction1()
{
    lv_scr_load(screenAction1);
    //生成动态页面的style
}
void show_screen_screenAction2()
{
    lv_scr_load(screenAction2);
    //生成动态页面的style
}
int screen_loop_enter = 0;
int _delay = 0;
void lv_qm_ui_loop(void)
{
#ifdef BLOCKLY_loop
    blockly_loop();
#endif
}

void lv_qm_ui_entry(void)
{

    /*
    * create screens
    */
    main_screen = lv_obj_create(NULL, NULL);
    screenConfig1 = lv_obj_create(NULL, NULL);
    screenConfig2 = lv_obj_create(NULL, NULL);
    screenConfig3 = lv_obj_create(NULL, NULL);
    screenAction1 = lv_obj_create(NULL, NULL);
    screenAction2 = lv_obj_create(NULL, NULL);

    lv_scr_load(main_screen);

    //CREATE STATIC OBJ
    lv_8ms_labelTempoReadAverage_create();
    lv_8ms_slider_create();
    lv_8ms_btn0_create();
    lv_8ms_labelTempoCompter_create();
    lv_8ms_labelTempoSet_create();
    lv_8ms_ButtonSetTempo_create();
    lv_8ms_configBtn_create();
    lv_8ms_label_5f0b_create();
    lv_8ms_label_cbd8_create();
    lv_8ms_btnForwardLoadZonesConfig1_create();
    lv_8ms_label_9580_create();
    lv_8ms_btnForwardConfig2_create();
    lv_8ms_ListZonesdropdown_create();
    lv_8ms_label_63c0_create();
    lv_8ms_label_3f53_create();
    lv_8ms_btnForwardConfig3_create();
    lv_8ms_btnActionBack1_create();
    lv_8ms_btnActionnext_create();
    lv_8ms_btnActionnext2_create();
    lv_8ms_list_of_actions_create();
    lv_8ms_list_action_2_create();

    static lv_style_t style_main_main_screen;
    lv_style_init(&style_main_main_screen);
    lv_style_set_bg_color(&style_main_main_screen, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    //lv_style_set_border_color(&style_main_main_screen,LV_STATE_DEFAULT,LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_add_style(main_screen, LV_CONT_PART_MAIN, &style_main_main_screen);

    static lv_style_t style_main_screenConfig1;
    lv_style_init(&style_main_screenConfig1);
    lv_style_set_bg_color(&style_main_screenConfig1, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    //lv_style_set_border_color(&style_main_screenConfig1,LV_STATE_DEFAULT,LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_add_style(screenConfig1, LV_CONT_PART_MAIN, &style_main_screenConfig1);

    static lv_style_t style_main_screenConfig2;
    lv_style_init(&style_main_screenConfig2);
    lv_style_set_bg_color(&style_main_screenConfig2, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    //lv_style_set_border_color(&style_main_screenConfig2,LV_STATE_DEFAULT,LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_add_style(screenConfig2, LV_CONT_PART_MAIN, &style_main_screenConfig2);

    static lv_style_t style_main_screenConfig3;
    lv_style_init(&style_main_screenConfig3);
    lv_style_set_bg_color(&style_main_screenConfig3, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    //lv_style_set_border_color(&style_main_screenConfig3,LV_STATE_DEFAULT,LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_obj_add_style(screenConfig3, LV_CONT_PART_MAIN, &style_main_screenConfig3);

    static lv_style_t style_main_screenAction1;
    lv_style_init(&style_main_screenAction1);
    lv_style_set_bg_color(&style_main_screenAction1, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
    //lv_style_set_border_color(&style_main_screenAction1,LV_STATE_DEFAULT,LV_COLOR_MAKE(0x58, 0x62, 0x73));
    lv_obj_add_style(screenAction1, LV_CONT_PART_MAIN, &style_main_screenAction1);

    static lv_style_t style_main_screenAction2;
    lv_style_init(&style_main_screenAction2);
    lv_style_set_bg_color(&style_main_screenAction2, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x58, 0x62, 0x73));
    //lv_style_set_border_color(&style_main_screenAction2,LV_STATE_DEFAULT,LV_COLOR_MAKE(0x58, 0x62, 0x73));
    lv_obj_add_style(screenAction2, LV_CONT_PART_MAIN, &style_main_screenAction2);

    init_function();
}
/**
* @brief blockly initialization
*/

void lvgl_blockly_init()
{
#ifdef WT_WIFI
    blockly_wifi_init();
#endif // DEBUG
#ifdef WT_BLUFI
    blockly_blufi_init();
#endif // DEBUG
#ifdef WT_HTTP_REQUEST
    blockly_http_init();
#endif // WT_HTTP_REQUEST
}

/**
* @brief blockly loop
*/

void lvgl_blockly_loop()
{
#ifdef WT_BLUFI
    blockly_blufi_loop();
#endif // DEBUG
#ifdef WT_WIFI
    blockly_wifi_loop();
#endif // DEBUG
#ifdef WT_HTTP_REQUEST
    blockly_http_loop();
#endif // WT_HTTP_REQUEST
}

void lvgl_device_init(void)
{
#ifdef ESP32_GPIO
    lv_8ms_gpio_setup();
#endif
#ifdef ESP32_TIMER
    lv_8ms_timer_setup();
#endif
#ifdef ESP32_UART
    lv_8ms_uart_setup();
#endif
}

void lvgl_device_loop(void)
{
#ifdef ESP32_GPIO
    lv_8ms_gpio_loop();
#endif
#ifdef ESP32_TIMER
    lv_8ms_timer_loop();
#endif
#ifdef ESP32_UART
    lv_8ms_uart_loop();
#endif
}