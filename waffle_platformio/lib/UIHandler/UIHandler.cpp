/*
 * UIHandler.cpp
 */

// #include <list>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "u8g2.h"
#include "u8g2_esp32_hal.h"

#include "UIHandler.h"
#include "graphics.h"

#define SSD1306
// #define LCD
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// // list of menu items
// std::list<MenuItem> menus;
// std::list<SubMenu> subMenus;

// u8g2_t u8g2; // a structure which will contain all the data for one display

UIHandler::UIHandler(){
    
}

int UIHandler::init(int PIN_SDA, int PIN_SCL, char *TAG = "UI", esp_log_level_t LOG_LEVEL = ESP_LOG_INFO) {
    TAG_UI = TAG;
    esp_log_level_set(TAG_UI, LOG_LEVEL);
    ESP_LOGI(TAG_UI, "Initializing UIHandler");
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;

#ifdef SSD1306
    u8g2_esp32_hal.bus.i2c.sda = (gpio_num_t)PIN_SDA;
    u8g2_esp32_hal.bus.i2c.scl = (gpio_num_t)PIN_SCL;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2, U8G2_R0,
        // u8x8_byte_sw_i2c,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb); // init u8g2 structure
    u8x8_SetI2CAddress(&u8g2.u8x8, 0x3c);
#endif

    ESP_LOGI(TAG_UI, "u8g2_InitDisplay");
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display
                             // is in sleep mode after this,
    UIHandler::wake();

    return 0;
}

int UIHandler::sleep() {
    ESP_LOGI(TAG_UI, "Putting display to sleep");
    u8g2_SetPowerSave(&u8g2, 1);
    return 0;
}

int UIHandler::wake() {
    //ESP_LOGI(TAG_UI, "Waking display");
    u8g2_SetPowerSave(&u8g2, 0);
    return 0;
}

int UIHandler::redraw() {
    ESP_LOGI(TAG_UI, "Redrawing UI");
    return 0;
}

int UIHandler::splashScreen() {
    ESP_LOGI(TAG_UI, "Displaying splash screen");
    ESP_LOGI(TAG_UI, "u8g2_ClearBuffer");
    u8g2_ClearBuffer(&u8g2);
    // ESP_LOGI(TAG_SCREEN, "u8g2_DrawBox");
    // u8g2_DrawBox(&u8g2, 0, 26, 80, 6);
    // u8g2_DrawFrame(&u8g2, 0, 26, 100, 6);

    ESP_LOGI(TAG_UI, "u8g2_DrawBitmap");
    u8g2_DrawBitmap(&u8g2, 0, 40, 128 / 8, 20, fredcorp_logo);

    ESP_LOGI(TAG_UI, "u8g2_SetFont");
    u8g2_SetFont(&u8g2, u8g2_font_pxplusibmvga9_t_all);
    ESP_LOGI(TAG_UI, "u8g2_DrawStr");
    u8g2_DrawStr(&u8g2, 18, 30, "fredcorp.cc");

    ESP_LOGI(TAG_UI, "u8g2_SendBuffer");
    u8g2_SendBuffer(&u8g2);

    ESP_LOGI(TAG_UI, "All done!");

    return 0;
}

int UIHandler::addMenu(int id, const char *name) {
    ESP_LOGI(TAG_UI, "Adding menu item %d: %s", id, name);
    return 0;
}

int UIHandler::removeMenu(int id) {
    ESP_LOGI(TAG_UI, "Removing menu item %d", id);
    return 0;
}

int UIHandler::selectMenu(int id) {
    ESP_LOGI(TAG_UI, "Selecting menu item %d", id);
    return 0;
}

int UIHandler::addMenuAction(int id, void (*action)(void)) {
    ESP_LOGI(TAG_UI, "Adding action to menu item %d", id);
    return 0;
}

int UIHandler::addSubMenu(int menuId, int id, const char *name) {
    ESP_LOGI(TAG_UI, "Adding sub menu %d to menu %d: %s", id, menuId, name);
    return 0;
}

int UIHandler::removeSubMenu(int menuId, int id) {
    ESP_LOGI(TAG_UI, "Removing sub menu %d from menu %d", id, menuId);
    return 0;
}

int UIHandler::selectSubMenu(int menuId, int id) {
    ESP_LOGI(TAG_UI, "Selecting sub menu %d from menu %d", id, menuId);
    return 0;
}

int UIHandler::addSubMenuAction(int menuId, int id, void (*action)(void)) {
    ESP_LOGI(TAG_UI, "Adding action to sub menu %d from menu %d", id, menuId);
    return 0;
}
