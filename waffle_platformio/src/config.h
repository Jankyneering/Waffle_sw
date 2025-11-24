#define CODE_REV "    dev    "

// Variables for radio
#define offset 0.0035       // device specific, in MHz. See README.md for more information on the matter.
#define frequency 439.98750 // Operational frequency
#define RSSI_DECAY 600      // Decay of the RSSI value in seconds

#define LCD
//#define SSD1306


// SDA - GPIO21
#define SDA 35
// SCL - GPIO22
// #define PIN_SCL 22
#define SCL 36

#define MOSI 11
#define SCK 12
#define CS 18
#define DC 21
#define RST 16

// #define LORA_SCK 5
// #define LORA_MISO 19
// #define LORA_MOSI 27
// #define LORA_SS 18
// #define LORA_DIO0 26
// #define LORA_DIO1 35
// #define LORA_DIO2 34
// #define LORA_RST 23

// #define LED GPIO_NUM_25
#define LED GPIO_NUM_5
#define UP GPIO_NUM_13
#define OK GPIO_NUM_15
#define DOWN GPIO_NUM_2
#define BUZZER GPIO_NUM_4