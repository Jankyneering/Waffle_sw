/*
 * UIHandler.h
 */

#ifndef UIHANDLER_H
#define UIHANDLER_H

#include "esp_log.h"
#include "u8g2.h"
#include <list>
#include <stdio.h>
#include <string.h>

#include "config.h"

class UIHandler {
public:
    UIHandler(int pin_sda, int pin_scl);
    UIHandler(int pin_mosi, int pin_sck, int pin_cs, int pin_dc, int pin_reset);
    ~UIHandler();
    int init(char *callsign, const char *tag, const char *code_rev, esp_log_level_t log_level);
    int sleep();
    int wake();
    int redraw();

    int blankScreen();
    int splashScreen();
    int showMenu(int menu);
    int getRedrawFlag();
    void setRSSI(int rssi);
    void setXTIME(bool xtime);
    int displayMessage(char message[32]);
    void setNewMessage(bool newMessage);
    void upButton(void *arg);
    void okButton(void *arg);
    void downButton(void *arg);

    bool sleepFlag = 0;

private:
    int _PIN_SDA;
    int _PIN_SCL;
    int _PIN_MOSI;
    int _PIN_SCK;
    int _PIN_CS;
    int _PIN_DC;
    int _PIN_RESET;
    char *_callsign;
    u8g2_t u8g2;
    const char *TAG_UI;
    esp_log_level_t _LOG_LEVEL;
    const char *_code_rev;
    int _menu;
    int _menuAmount = 3;
    int _redrawFlag = 0;
    bool _gotXTIME  = false;
    void drawRSSIbars(u8g2_t _u8g2, u8g2_uint_t x, u8g2_uint_t y, int rssi);
    void createMenuBar(u8g2_t _u8g2, const char *title);
    int _rssi         = -180;
    char _message[32] = "No message";
    bool _newMessage  = 0;
};

#endif