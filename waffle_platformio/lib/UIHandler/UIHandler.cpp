/*
 * UIHandler.cpp
 */

#include "u8g2.h"
#include "u8g2_esp32_hal.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <fstream>
#include <sdkconfig.h>
#include <stdio.h>
#include <string.h>

#include "UIHandler.h"
#include "graphics.h"

#define SSD1306
// #define LCD
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const int menuAmount = 3;

UIHandler::UIHandler(int PIN_SDA, int PIN_SCL) {
    _PIN_SDA = PIN_SDA;
    _PIN_SCL = PIN_SCL;
}

int UIHandler::init(char *callsign, const char *TAG, const char *CODE_REV, esp_log_level_t LOG_LEVEL = ESP_LOG_INFO) {
    _callsign  = callsign;
    TAG_UI     = TAG;
    _LOG_LEVEL = LOG_LEVEL;
    _code_rev  = CODE_REV;
    esp_log_level_set(TAG_UI, _LOG_LEVEL);
    ESP_LOGI(TAG_UI, "Initializing UIHandler");
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;

#ifdef SSD1306
    u8g2_esp32_hal.bus.i2c.sda = (gpio_num_t)_PIN_SDA;
    u8g2_esp32_hal.bus.i2c.scl = (gpio_num_t)_PIN_SCL;
    u8g2_esp32_hal.clk_speed   = 400000;
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
    // ESP_LOGI(TAG_UI, "Waking display");
    u8g2_SetPowerSave(&u8g2, 0);
    return 0;
}

int UIHandler::redraw() {
    ESP_LOGI(TAG_UI, "Redrawing UI");
    showMenu(_menu);
    return 0;
}

int UIHandler::blankScreen() {
    ESP_LOGI(TAG_UI, "Blanking screen");
    u8g2_ClearBuffer(&u8g2);
    u8g2_SendBuffer(&u8g2);
    return 0;
}

int UIHandler::splashScreen() {
    ESP_LOGI(TAG_UI, "Displaying splash screen");
    ESP_LOGI(TAG_UI, "u8g2_ClearBuffer");
    u8g2_ClearBuffer(&u8g2);

    // ESP_LOGI(TAG_UI, "u8g2_DrawBitmap");
    // u8g2_DrawBitmap(&u8g2, 0, 40, 128 / 8, 20, fredcorp_logo);
    // ESP_LOGI(TAG_UI, "u8g2_DrawBitmap");
    // u8g2_DrawBitmap(&u8g2, 32, 4, 8, 54, wednesday_logo);

    // ESP_LOGI(TAG_UI, "u8g2_SetFont");
    // u8g2_SetFont(&u8g2, u8g2_font_pxplusibmvga9_t_all);
    // ESP_LOGI(TAG_UI, "u8g2_DrawStr");
    // u8g2_DrawStr(&u8g2, 18, 30, "fredcorp.cc");

    ESP_LOGI(TAG_UI, "u8g2_DrawBitmap");
    u8g2_DrawBitmap(&u8g2, 0, 0, 8, 54, waffle_logo);
    ESP_LOGI(TAG_UI, "u8g2_SetFont");
    u8g2_SetFont(&u8g2, u8g2_font_6x13_mf);
    ESP_LOGI(TAG_UI, "u8g2_DrawStr");
    u8g2_DrawStr(&u8g2, 0, 64, _code_rev);
    u8g2_DrawStr(&u8g2, 72, 16, "Waffle");
    u8g2_DrawStr(&u8g2, 72, 28, "POGSAG");
    u8g2_DrawStr(&u8g2, 72, 44, _callsign);
    char resetReason[32];
    esp_reset_reason_t reset_reason = esp_reset_reason();
    ESP_LOGI(TAG_UI, "Reset reason: %d", reset_reason);
    switch (reset_reason) {
    case ESP_RST_UNKNOWN:
        strcpy(resetReason, "Unknown");
        break;
    case ESP_RST_POWERON:
        strcpy(resetReason, "Power on");
        break;
    case ESP_RST_EXT:
        strcpy(resetReason, "External");
        break;
    case ESP_RST_SW:
        strcpy(resetReason, "Software");
        break;
    case ESP_RST_PANIC:
        strcpy(resetReason, "Panic");
        break;
    case ESP_RST_INT_WDT:
        strcpy(resetReason, "Watchdog");
        break;
    case ESP_RST_TASK_WDT:
        strcpy(resetReason, "TWatchdog");
        break;
    case ESP_RST_WDT:
        strcpy(resetReason, "Watchdog");
        break;
    case ESP_RST_DEEPSLEEP:
        strcpy(resetReason, "Deep sleep");
        break;
    case ESP_RST_BROWNOUT:
        strcpy(resetReason, "Brownout");
        break;
    case ESP_RST_SDIO:
        strcpy(resetReason, "SDIO");
        break;
    case ESP_RST_JTAG:
        strcpy(resetReason, "JTAG");
        break;
    case ESP_RST_USB:
        strcpy(resetReason, "USB");
        break;
    case ESP_RST_EFUSE:
        strcpy(resetReason, "EFUSE");
        break;
    case ESP_RST_PWR_GLITCH:
        strcpy(resetReason, "Power glitch");
        break;
    case ESP_RST_CPU_LOCKUP:
        strcpy(resetReason, "CPU lockup");
        break;
    }

    u8g2_DrawStr(&u8g2, 72, 56, resetReason);

    ESP_LOGI(TAG_UI, "u8g2_SendBuffer");
    u8g2_SendBuffer(&u8g2);

    // ESP_LOGI(TAG_UI, "All done!");

    return 0;
}

void UIHandler::setRSSI(int rssi) {
    _rssi = rssi;
    redraw();
}

void UIHandler::setXTIME(bool xtime) {
    _gotXTIME = xtime;
    redraw();
}

int UIHandler::displayMessage(char message[32]) {
    // constrains message to 32 characters
    strncpy(_message, message, 32);
    showMenu(0);
    return 0;
}

void UIHandler::setNewMessage(bool newMessage) {
    _newMessage = newMessage;
    redraw();
}

void UIHandler::drawRSSIbars(u8g2_t _u8g2, u8g2_uint_t x, u8g2_uint_t y, int rssi) {
    switch (rssi) {
    case -200 ... - 175:
        u8g2_DrawBox(&_u8g2, x, y + 8, 3, 1);
        u8g2_DrawBox(&_u8g2, x + 4, y + 8, 3, 1);
        u8g2_DrawBox(&_u8g2, x + 8, y + 8, 3, 1);
        u8g2_DrawBox(&_u8g2, x + 12, y + 8, 3, 1);
        break;
    case -174 ... - 151:
        u8g2_DrawBox(&_u8g2, x, y + 8, 3, 1);
        u8g2_DrawBox(&_u8g2, x + 4, y + 8, 3, 1);
        u8g2_DrawBox(&_u8g2, x + 8, y + 8, 3, 1);
        u8g2_DrawBox(&_u8g2, x + 12, y + 6, 3, 3);
        break;
    case -150 ... - 127:
        u8g2_DrawBox(&_u8g2, x, y + 8, 3, 1);
        u8g2_DrawBox(&_u8g2, x + 4, y + 8, 3, 1);
        u8g2_DrawBox(&_u8g2, x + 8, y + 4, 3, 5);
        u8g2_DrawBox(&_u8g2, x + 12, y + 6, 3, 3);
        break;
    case -126 ... - 103:
        u8g2_DrawBox(&_u8g2, x, y + 8, 3, 1);
        u8g2_DrawBox(&_u8g2, x + 4, y + 2, 3, 7);
        u8g2_DrawBox(&_u8g2, x + 8, y + 4, 3, 5);
        u8g2_DrawBox(&_u8g2, x + 12, y + 6, 3, 3);
        break;
    case -102 ... 0:
        u8g2_DrawBox(&_u8g2, x, y, 3, 9);
        u8g2_DrawBox(&_u8g2, x + 4, y + 2, 3, 7);
        u8g2_DrawBox(&_u8g2, x + 8, y + 4, 3, 5);
        u8g2_DrawBox(&_u8g2, x + 12, y + 6, 3, 3);
        break;
    default:
        ESP_LOGW(TAG_UI, "RSSI draw failed");
        break;
    }
}

void UIHandler::createMenuBar(u8g2_t _u8g2, const char *title) {
    u8g2_DrawLine(&_u8g2, 0, 12, 127, 12);
    if (_newMessage) {
        u8g2_DrawBitmap(&_u8g2, 94, 2, 2, 7, envelope);
    }
    drawRSSIbars(_u8g2, 110, 1, _rssi);

    if (_gotXTIME) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm *tm = localtime(&tv.tv_sec);
        char timeString[6];
        sprintf(timeString, "%02d:%02d", tm->tm_hour, tm->tm_min);
        u8g2_SetFont(&_u8g2, u8g2_font_6x13_mf);
        u8g2_DrawStr(&_u8g2, 50, 10, timeString);
    }
    u8g2_SetFont(&_u8g2, u8g2_font_6x13_mf);
    u8g2_DrawStr(&_u8g2, 0, 10, title);
}

int UIHandler::showMenu(int menu) {
    _menu = menu;
    ESP_LOGI(TAG_UI, "Displaying menu %d", _menu);
    u8g2_ClearBuffer(&u8g2);

    switch (_menu) {
    case 0:
        ESP_LOGI(TAG_UI, "main");
        createMenuBar(u8g2, _callsign);
        u8g2_SetFont(&u8g2, u8g2_font_6x13_mf);
        // u8g2_SetFont(&u8g2, u8g2_font_inb16_mf);
        u8g2_DrawStr(&u8g2, 0, 40, _message);
        break;
    case 1:
        ESP_LOGI(TAG_UI, "Menu 1");
        createMenuBar(u8g2, "Menu 1");
        u8g2_SetFont(&u8g2, u8g2_font_inb16_mf);
        u8g2_DrawStr(&u8g2, 0, 40, "Item 1");
        break;
    case 2:
        ESP_LOGI(TAG_UI, "Menu 2");
        createMenuBar(u8g2, "Menu 2");
        u8g2_SetFont(&u8g2, u8g2_font_inb16_mf);
        u8g2_DrawStr(&u8g2, 0, 40, "Item 2");
        break;
    case 3:
        ESP_LOGI(TAG_UI, "Menu 3");
        createMenuBar(u8g2, "Menu 3");
        u8g2_SetFont(&u8g2, u8g2_font_inb16_mf);
        u8g2_DrawStr(&u8g2, 0, 40, "Item 3");
        break;
    default:
        ESP_LOGI(TAG_UI, "Unknown menu");
        break;
    }

    u8g2_SendBuffer(&u8g2);
    return 0;
}

int UIHandler::getRedrawFlag() {
    int ret = _redrawFlag;
    _redrawFlag--;
    if (_redrawFlag < 0) {
        _redrawFlag = 0;
    }
    return ret;
}

void UIHandler::upButton(void *arg) {
    // code for up
    _menu--;
    if (_menu < 0) {
        _menu = _menuAmount;
    }
    _redrawFlag++;
}

void UIHandler::okButton(void *arg) {
    // code for ok
    sleepFlag = !sleepFlag;
    _redrawFlag++;
}

void UIHandler::downButton(void *arg) {
    // code for down
    _menu++;
    if (_menu > _menuAmount) {
        _menu = 0;
    }
    _redrawFlag++;
}