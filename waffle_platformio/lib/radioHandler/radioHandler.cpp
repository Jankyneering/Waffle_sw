#include <esp_log.h>
#include <radioHandler.h>
#include <RadioLib.h>


radioHandler::radioHandler(int LORA_SCK, int LORA_MISO, int LORA_MOSI, int LORA_SS, int LORA_DIO0, int LORA_DIO1, int LORA_DIO2, int LORA_RST){
    _LORA_SCK = LORA_SCK;
    _LORA_MISO = LORA_MISO;
    _LORA_MOSI = LORA_MOSI;
    _LORA_SS = LORA_SS;
    _LORA_DIO0 = LORA_DIO0;
    _LORA_DIO1 = LORA_DIO1;
    _LORA_DIO2 = LORA_DIO2;
    _LORA_RST = LORA_RST;
    _hal = new EspHal(_LORA_SCK, _LORA_MISO, _LORA_MOSI);
    _radio = new Module(_hal, _LORA_SS, _LORA_DIO0, _LORA_RST, _LORA_DIO1);
    _pager = new PagerClient(_radio);

}

void radioHandler::pocsagInit(float frequency, float offset, char *TAG) {
    TAG_RADIO = TAG;
    _frequency = frequency;
    _offset = offset;

    // initialize SX1278 with default settings
    ESP_LOGI(TAG_RADIO, "[SX1278] Initializing ... "); // Print a message to the console
    int state = _radio.beginFSK();                      // Initialize the radio module

    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "success!"); // Report success
    } else {
        ESP_LOGE(TAG_RADIO, "failed");
        while (true)
            ; // Halt
    }

    // initialize Pager client
    ESP_LOGI(TAG_RADIO, "[Pager] Initializing ... "); // Print a message to the console
    state = _pager.begin(_frequency + _offset, 1200);  // Initialize the pager client
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
    int state = _pager.startReceive(_LORA_DIO2, 200, 0); // Start receiving messages
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "success!"); // Report success
    } else {
        ESP_LOGE(TAG_RADIO, "failed "); // Report error
        while (true)
            ; // Halt
    }
}

void radioHandler::pocsagSendText(int txRic, char *txText) {
    int state = _pager.transmit(txText, txRic, RADIOLIB_PAGER_ASCII);
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "success!");
    } else {
        ESP_LOGE(TAG_RADIO, "failed");
    }
}

void radioHandler::pocsagSendNum(int txRic, char *txNum) {
    int state = _pager.transmit(txNum, txRic);
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG_RADIO, "success!");
    } else {
        ESP_LOGE(TAG_RADIO, "failed");
    }
}
