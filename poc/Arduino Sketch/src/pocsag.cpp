#include <pocsag.h>
#include <config.h>

void pocsagInit() {
    // initialize SX1278 with default settings
    Serial.print(F("[SX1278] Initializing ... ")); // Print a message to the serial port
    int state = radio.beginFSK(); // Initialize the radio module

    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!")); // Report success
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state); // Report error
        while (true); // Halt
    }

    // initialize Pager client
    Serial.print(F("[Pager] Initializing ... ")); // Print a message to the serial port
    state = pager.begin(frequency + offset, 1200); // Initialize the pager client
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!")); // Report success
    } else {
        Serial.print(F("failed, code ")); // Report error
        Serial.println(state); // Report error
        while (true); // Halt
    }
}

void pocsagStartRx() {
    // start receiving POCSAG messages
    Serial.print(F("[Pager] Starting to listen ... ")); // Print a message to the serial port
    // address of this "pager":     1234567
    int state = pager.startReceive(LORA_DIO2, 200, 0); // Start receiving messages
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!")); // Report success
    } else {
        Serial.print(F("failed, code ")); // Report error
        Serial.println(state); // Report error
        while (true); // Halt
    }
}

void pocsagSendText(int tx_ric, String tx_text) {
  int state = pager.transmit(tx_text,tx_ric,RADIOLIB_PAGER_ASCII);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }
}

void pocsagSendNum(int tx_ric, String tx_num) {
  int state = pager.transmit(tx_num,tx_ric);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }
}
