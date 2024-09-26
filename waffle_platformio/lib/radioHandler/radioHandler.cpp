#include <esp_log.h>
#include <radioHandler.h>




radioHandler::radioHandler(){

}

void radioHandler::pocsagInit(float frequency, float offset, char *TAG) {
    TAG_RADIO = TAG;
    _frequency = frequency;
    _offset = offset;

    // initialize SX1278 with default settings
    ESP_LOGI(TAG_RADIO, "[SX1278] Initializing ... "); // Print a message to the console
    int state = radio.beginFSK();                      // Initialize the radio module

    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "success!"); // Report success
    } else {
        ESP_LOGE(TAG_RADIO, "failed");
        while (true)
            ; // Halt
    }

    // initialize Pager client
    ESP_LOGI(TAG_RADIO, "[Pager] Initializing ... "); // Print a message to the console
    state = pager.begin(_frequency + _offset, 1200);  // Initialize the pager client
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "success!"); // Report success
    } else {
        ESP_LOGE(TAG_RADIO, "failed"); // Report error
        while (true)
            ; // Halt
    }
}

void radioHandler::pocsagStartRx() {
    // start receiving POCSAG messages
    ESP_LOGI(TAG_RADIO, "[Pager] Starting to listen ... "); // Print a message to the console
    // address of this "pager":     1234567
    int state = pager.startReceive(LORA_DIO2, 200, 0); // Start receiving messages
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "success!"); // Report success
    } else {
        ESP_LOGE(TAG_RADIO, "failed "); // Report error
        while (true)
            ; // Halt
    }
}

void radioHandler::pocsagSendText(int txRic, char *txText) {
    int state = pager.transmit(txText, txRic, RADIOLIB_PAGER_ASCII);
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "success!");
    } else {
        ESP_LOGE(TAG_RADIO, "failed");
    }
}

void radioHandler::pocsagSendNum(int txRic, char *txNum) {
    int state = pager.transmit(txNum, txRic);
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "success!");
    } else {
        ESP_LOGE(TAG_RADIO, "failed");
    }
}
