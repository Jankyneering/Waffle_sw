/*
 * UIHandler.h
 */

#ifndef UIHANDLER_H
#define UIHANDLER_H

#include "esp_log.h"
#include "u8g2.h"
#include <stdio.h>
#include <string.h>
#include <list>


class UIHandler {
public:
    UIHandler(char *callsign);
    int init(int PIN_SDA, int PIN_SCL, char *TAG, esp_log_level_t LOG_LEVEL);
    int sleep();
    int wake();
    int redraw();

    int splashScreen();
    int showMenu(int menu);
    void setRSSI(int rssi);

private:
    char *_callsign; // Pointer to a character array (string)
    u8g2_t u8g2; // a structure which will contain all the data for one display
    char *TAG_UI;
    void drawRSSIbars(u8g2_t _u8g2, u8g2_uint_t x, u8g2_uint_t y, int rssi);
    int _rssi = 0;
};

#endif