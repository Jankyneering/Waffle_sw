#define CODE_REV "    dev    "

// Variables for radio
#define offset 0.0035           // device specific, in MHz. See README.md for more information on the matter.
#define frequency 439.98750     // Operational frequency
#define RSSI_DECAY 600          // Decay of the RSSI value in seconds
#define BACKLIGHT_SLEEP_TIME 10 // Time in seconds before the backlight goes off
#define DISPLAY_SLEEP_TIME 30   // Time in seconds before the display goes to sleep

/*
 * Display selection
 * Only one display type can be selected at a time.
 */
#ifdef SSD1306
#undef ST7567
#endif

#ifdef ST7567
#undef SSD1306
#endif

/*
 * Pin definitions for the LILIGO TTGO LoRa32 board.
 */
#ifdef TTGO
#define PIN_SDA 21
#define PIN_SCL 22
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_DIO0 26
#define LORA_DIO1 35
#define LORA_DIO2 34
#define LORA_RST 23
#define LED GPIO_NUM_25
#define UP GPIO_NUM_13
#define OK GPIO_NUM_15
#define DOWN GPIO_NUM_2
#define BUZZER GPIO_NUM_4
#endif

/*
 * Pin definitions for the Lolin S3 Mini board.
 ! Currently under development, pins may change !
 */
#ifdef S3MINI
#define PIN_SDA 35
#define PIN_SCL 36

#define MOSI 11
#define SCK 12
#define LCD_CS 18
#define LCD_DC 21
#define LCD_RST 16
#define LCD_BACKLIGHT 17

#define LED GPIO_NUM_5
#define UP GPIO_NUM_13
#define OK GPIO_NUM_15
#define DOWN GPIO_NUM_2
#define BUZZER GPIO_NUM_4
#endif
