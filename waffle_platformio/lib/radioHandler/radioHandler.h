/*
 * radioHandler.h
 * Takes care of all radiocomm aspects (for now, only via an SX1278 via Radiolib)
 */

#ifndef RADIOHANDLER_H
#define RADIOHANDLER_H

#include "EspHal.h"
#include "esp_log.h"
#include <RadioLib.h>


// create a new instance of the HAL class
// EspHal *hal = new EspHal(LORA_SCK, LORA_MISO, LORA_MOSI);
// create radio module instance
// SX1278 radio = new Module(hal, LORA_SS, LORA_DIO0, LORA_RST, LORA_DIO1); // Radio module instance
// create Pager client instance using the FSK module
// PagerClient pager(&radio); // Pager client instance


class radioHandler {
public:
    radioHandler(int LORA_SCK, int LORA_MISO, int LORA_MOSI, int LORA_SS, int LORA_DIO0, int LORA_DIO1, int LORA_DIO2, int LORA_RST);
    void pocsagInit(float frequency, float offset, char *TAG = "RADIO");
    void pocsagStartRx();
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
    EspHal *_hal;
    SX1278 _radio;
    PagerClient _pager;
    char *TAG_RADIO;
    float _frequency;
    float _offset;
    int _txRic;
    char *_txText;
    char *_txNum;
};

#endif