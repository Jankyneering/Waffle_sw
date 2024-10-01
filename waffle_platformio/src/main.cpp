#include "UIHandler.h"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <driver/i2c_master.h>
#include <driver/spi_master.h>
#include <esp_log.h>
// #include <esp_vfs.h> // Include this header for esp_spiffs_dir_entry_t
#include <stdio.h>

#include "radioHandler.h"

#include "config.h"

extern "C" {
void app_main(void);
}

static bool ledState = false;

static const UBaseType_t UITaskPriority = 2;
static const UBaseType_t taskPriority = 1;

static const char *TAG_MAIN = "[MAIN      ]";
static const char *TAG_UI = "[UI_Handler]";
static const char *TAG_LED = "[LED       ]";
static const char *TAG_RADIO = "[RADIO     ]";
static const char *TAG_SPIFFS = "[SPIFFS    ]";

UIHandler uiHandler(PIN_SDA, PIN_SCL);
radioHandler sxradio(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS, LORA_DIO0, LORA_DIO1, LORA_DIO2, LORA_RST);

void vUITask(void *pvParameters) {
    uiHandler.splashScreen();

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    uiHandler.showMenu(0);
    for (;;) {
        // uiHandler.showMenu(0);
        // uiHandler.setRSSI(0);
        // vTaskDelay(500 / portTICK_PERIOD_MS);
        // uiHandler.setRSSI(12);
        // vTaskDelay(500 / portTICK_PERIOD_MS);
        // uiHandler.setRSSI(42);
        // vTaskDelay(500 / portTICK_PERIOD_MS);
        // uiHandler.setRSSI(69);
        // vTaskDelay(500 / portTICK_PERIOD_MS);
        // uiHandler.setRSSI(100);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        // uiHandler.setNewMessage(true);
        // for (int i = 0; i <= 3; i++) {
        //     uiHandler.showMenu(i);
        //     vTaskDelay(1500 / portTICK_PERIOD_MS);
        // }
        // uiHandler.setNewMessage(false);
    }
    vTaskDelete(NULL);
}

void vRadioTask(void *pvParameters) {
    ESP_LOGI(TAG_RADIO, "Radio is running");
    ESP_LOGI(TAG_RADIO, "Radio is running on core %d\n", xPortGetCoreID());

    // sxradio.pocsagSendText(100, "TEST");

    sxradio.pocsagStartRx();

    char *message = new char[32];
    int *address = new int;
    char *fullMessage = new char[32];
    for (;;) {
        if (sxradio.pocsagAvailable() >= 2) {
            sxradio.pocsagGetMessage(address, message);
            // Concatenate the address and the message
            sprintf(fullMessage, "%d: %s", *address, message);
            uiHandler.displayMessage(fullMessage);
            int RSSI = sxradio.getRSSI();
            ESP_LOGI(TAG_RADIO, "RSSI : %d", RSSI);
            uiHandler.setRSSI(RSSI);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void vLedTask(void *pvParameters) {
    for (;;) {
        ESP_LOGI(TAG_LED, "LedTask is running");
        ESP_LOGI(TAG_LED, "LedTask is running on core %d\n", xPortGetCoreID());
        ledState = !ledState;
        gpio_set_level(LED, ledState);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void vSPIFFSTask(void *pvParameters) {
    // Show all files in spiffs
    esp_vfs_spiffs_conf_t config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_vfs_spiffs_register(&config);

    FILE *file = fopen("/spiffs/text.txt", "r");
    if (file == NULL) {
        ESP_LOGE(TAG_SPIFFS, "File does not exist!");
    } else {
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL) {
            ESP_LOGI(TAG_SPIFFS, "%s", line);
        }
        fclose(file);
    }
    vTaskDelete(NULL);
}

void app_main() {
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    uiHandler.init(CALLSIGN, TAG_UI, ESP_LOG_WARN);
    sxradio.pocsagInit(frequency, offset, TAG_RADIO, ESP_LOG_INFO);

    esp_log_level_set(TAG_MAIN, ESP_LOG_WARN);
    esp_log_level_set(TAG_LED, ESP_LOG_WARN);
    esp_log_level_set(TAG_SPIFFS, ESP_LOG_INFO);

    xTaskCreate(vRadioTask,
                "RadioTask",
                100000,
                NULL,
                taskPriority,
                NULL);
    xTaskCreate(vLedTask,       // Entry function of the task
                "LedTask",      // Name of the task
                5000,           // The number of words to allocate for use as the task's
                                // stack (arbitrary size enough for this task)
                NULL,           // No parameter passed to the task
                taskPriority,   // Priority of the task
                NULL);          // No handle
    xTaskCreate(vUITask,        // Entry function of the task
                "UITask",       // Name of the task
                100000,         // The number of words to allocate for use as the task's
                                // stack (arbitrary size enough for this task)
                NULL,           // No parameter passed to the task
                UITaskPriority, // Priority of the task
                NULL);          // No handle
    xTaskCreate(vSPIFFSTask,    // Entry function of the task
                "SPIFFSTask",   // Name of the task
                10000,          // The number of words to allocate for use as the task's
                                // stack (arbitrary size enough for this task)
                NULL,           // No parameter passed to the task
                taskPriority,   // Priority of the task
                NULL);          // No handle

    for (;;) {
        // Display the core on which the main function is running
        ESP_LOGI(TAG_MAIN, "app_main() is running on core %d\n", xPortGetCoreID());
        // Wait 1 seconds
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
