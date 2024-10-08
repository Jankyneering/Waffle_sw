// #include "RadioLib.h"
#include "radioHandler.h"
#include "RadioLib_esp32_hal.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <esp_log.h>

radioHandler::radioHandler(int LORA_SCK, int LORA_MISO, int LORA_MOSI, int LORA_SS, int LORA_DIO0, int LORA_DIO1, int LORA_DIO2, int LORA_RST) {
    _LORA_SCK = LORA_SCK;
    _LORA_MISO = LORA_MISO;
    _LORA_MOSI = LORA_MOSI;
    _LORA_SS = LORA_SS;
    _LORA_DIO0 = LORA_DIO0;
    _LORA_DIO1 = LORA_DIO1;
    _LORA_DIO2 = LORA_DIO2;
    _LORA_RST = LORA_RST;
    _hal = new RadioLib_esp32_hal(_LORA_SCK, _LORA_MISO, _LORA_MOSI);
    _radio = new Module(_hal, _LORA_SS, _LORA_DIO0, _LORA_RST, _LORA_DIO1);
    _pager = new PagerClient(&_radio);
}

void radioHandler::pocsagInit(float frequency, float offset, const char *TAG, esp_log_level_t LOG_LEVEL) {
    TAG_RADIO = TAG;
    _frequency = frequency;
    _offset = offset;
    _LOG_LEVEL = LOG_LEVEL;
    esp_log_level_set(TAG_RADIO, LOG_LEVEL);

    // initialize SX1278 with default settings
    ESP_LOGD(TAG_RADIO, "[SX1278] Initializing ... "); // Print a message to the console
    int state = _radio.beginFSK();                     // Initialize the radio module

    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGD(TAG_RADIO, "SX1278 initsuccess!"); // Report success
    } else {
        ESP_LOGE(TAG_RADIO, "SX1278 init failed");
        while (true)
            ; // Halt
    }

    // initialize Pager client
    ESP_LOGD(TAG_RADIO, "[Pager] Initializing ... ");  // Print a message to the console
    state = _pager->begin(_frequency + _offset, 1200); // Initialize the pager client
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGD(TAG_RADIO, "Pager init success!"); // Report success
    } else {
        ESP_LOGE(TAG_RADIO, "Pager init failed"); // Report error
        while (true)
            ; // Halt
    }
}

int radioHandler::pocsagStartRx() {
    // start receiving POCSAG messages
    ESP_LOGD(TAG_RADIO, "[Pager] Starting to listen ... "); // Print a message to the console
    // address of this "pager":     1234567
    int state = _pager->startReceive(_LORA_DIO2, 200, 0); // Start receiving messages
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGD(TAG_RADIO, "Pager RX start success!"); // Report success
        return 0;
    } else {
        ESP_LOGE(TAG_RADIO, "Pager RX start failed "); // Report error
        return -1;
    }
}

int radioHandler::getRSSI() {
    int RSSI = _radio.getRSSI();
    //pocsagStartRx();
    return RSSI;
}

int radioHandler::pocsagAvailable() {
    ESP_LOGV(TAG_RADIO, "POCSAG Available: %d", _pager->available());
    return _pager->available();
}

int radioHandler::pocsagGetMessage(int *address, char *message) {
    ESP_LOGD(TAG_RADIO, "Reading POCSAG data");
    uint8_t str[32]; // Adjust size as needed
    uint32_t addr = 0;
    size_t len = 32;
    int state = _pager->readData(str, &len, &addr);
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "Pocsag RX success!");
        ESP_LOGD(TAG_RADIO, "Message: %s", str);
        ESP_LOGD(TAG_RADIO, "Address: %d", (int)addr);
        *address = (int)addr;
        ESP_LOGD(TAG_RADIO, "Strncpy operation");
        strncpy(message, (char *)str, 32);
        return 0;
    } else {
        ESP_LOGE(TAG_RADIO, "Pocsag RX failed");
        return -1;
    }
}

void radioHandler::pocsagSendText(int txRic, char *txText) {
    int state = _pager->transmit(txText, txRic, RADIOLIB_PAGER_ASCII);
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "Pocsag TX success!");
    } else {
        ESP_LOGE(TAG_RADIO, "Pocsag TX failed");
    }
}

void radioHandler::pocsagSendNum(int txRic, char *txNum) {
    int state = _pager->transmit(txNum, txRic);
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "Pocsag TX num success!");
    } else {
        ESP_LOGE(TAG_RADIO, "Pocsag TX num failed");
    }
}
