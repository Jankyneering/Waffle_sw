# ESP32-Pocsag-Pager
 Proof-of-concept Pager using a TTGO dev board. Will be merged with the "Waffle" project someday.
 
# Libraries
 * [RadioLib](https://github.com/jgromes/RadioLib)
 * Adafruit GFX (I plan to move tu U8G2 at some point)
 * Adafruiy SSD1306
 
 
# Hardware
Uses an ESP32 LORA32 TTGO. You must bridge DIO1 to pin 35 and DIO2 to pin 34 (broken out as LORA1 and LORA2 on the headers). 
I added a buzzer on pin 14, and plan to add buttons in the near future

# Setup
Ideally, you should calibrate your SX1278 as it probably has an offset. Right now, best way would be to use a TXCO SDR and use another RadioLib sketch example to transmit a continuous signal and determine the offset from there.
In config.h, change the RIC with yours, fiddle with the tones, and enjoy!

# Next Steps
* Write.Documentation.
* ~~Add multi-address reception~~ Done.
* ~~Configurable ringtones~~ Done.
* Implement a clean user interface to browse config, messages, etc. Currently experimenting with ArduinoMenu
* Create a wifi configuration portal
* Implement RTC
* Settings storage in SPIFFS
* Message storage in SPIFFS
