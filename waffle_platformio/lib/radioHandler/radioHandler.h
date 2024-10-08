/*
 * radioHandler.h
 * Takes care of all radiocomm aspects (for now, only via an SX1278 via Radiolib)
 */

#ifndef RADIOHANDLER_H
#define RADIOHANDLER_H

#include "RadioLib_esp32_hal.h"
#include "RadioLib.h"

// create a new instance of the HAL class
// EspHal *hal = new EspHal(LORA_SCK, LORA_MISO, LORA_MOSI);
// create radio module instance
// SX1278 radio = new Module(hal, LORA_SS, LORA_DIO0, LORA_RST, LORA_DIO1); // Radio module instance
// create Pager client instance using the FSK module
// PagerClient pager(&radio); // Pager client instance

class radioHandler {
public:
    radioHandler(int LORA_SCK, int LORA_MISO, int LORA_MOSI, int LORA_SS, int LORA_DIO0, int LORA_DIO1, int LORA_DIO2, int LORA_RST);
    void pocsagInit(float frequency, float offset, const char *TAG = "RADIO", esp_log_level_t LOG_LEVEL = ESP_LOG_INFO);
    int pocsagStartRx();
    int pocsagAvailable();
    int pocsagGetMessage(int *address, char *message);
    int getRSSI();
    void pocsagSendText(int txRic, char *txText);
    void pocsagSendNum(int txRic, char *txNum);

private:
    int _LORA_SCK;
    int _LORA_MISO;
    int _LORA_MOSI;
    int _LORA_SS;
    int _LORA_DIO0;
    int _LORA_DIO1;
    int _LORA_DIO2;
    int _LORA_RST;
    RadioLib_esp32_hal *_hal;// = new RadioLib_esp32_hal(_LORA_SCK, _LORA_MISO, _LORA_MOSI);
    SX1278 _radio = new Module(_hal, _LORA_SS, _LORA_DIO0, _LORA_RST, _LORA_DIO1);
    PagerClient *_pager;
    const char *TAG_RADIO;
    esp_log_level_t _LOG_LEVEL;
    float _frequency;
    float _offset;
    int _txRic;
    char *_txText;
    char *_txNum;
};

#endif