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

class UIHandler {
public:
    UIHandler(int PIN_SDA, int PIN_SCL);
    int init(char *callsign, const char *TAG, esp_log_level_t LOG_LEVEL);
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
    char *_callsign; // Pointer to a character array (string)
    u8g2_t u8g2;     // a structure which will contain all the data for one display
    const char *TAG_UI;
    esp_log_level_t _LOG_LEVEL;
    int _menu;
    int _menuAmount = 3;
    int _redrawFlag = 0;
    bool _gotXTIME   = false;
    void drawRSSIbars(u8g2_t _u8g2, u8g2_uint_t x, u8g2_uint_t y, int rssi);
    int _rssi         = -180;
    char _message[32] = "No message";
    bool _newMessage  = 0;
};

#endif