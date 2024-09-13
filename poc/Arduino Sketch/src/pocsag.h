#include <Arduino.h>
#include <periph.h>
//#include "config.h"
#include <RadioLib.h>
#include <SPI.h>
#include <Wire.h>
#include <Wifi.h>

SX1278 radio = new Module(LORA_SS, LORA_DIO0, LORA_RST, LORA_DIO1); // Radio module instance
// create Pager client instance using the FSK module
PagerClient pager(&radio); // Pager client instance



void pocsagInit();
void pocsagStartRx();
void pocsagSendText(int tx_ric, String tx_text);
void pocsagSendNum(int tx_ric, String tx_num);