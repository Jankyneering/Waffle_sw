#include <cJSON.h>
#include <driver/gpio.h>
#include <driver/i2c_master.h>
#include <driver/spi_master.h>
#include <esp_chip_info.h>
#include <esp_log.h>
#include <esp_spiffs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <fstream>
#include <sdkconfig.h>
#include <stdio.h>

#include "UIHandler.h"
#include "radioHandler.h"

#include "config.h"

extern "C" {
void app_main(void);
}

static bool ledState = false;

char CALLSIGN[32];
int ADDRESSES[10][2];

static const UBaseType_t UITaskPriority    = 2;
static const UBaseType_t RadioTaskPriority = 3;
static const UBaseType_t taskPriority      = 1;

static const char *TAG_MAIN                = "[MAIN      ]";
static const char *TAG_UI                  = "[UI_Handler]";
static const char *TAG_LED                 = "[LED       ]";
static const char *TAG_RADIO               = "[RADIO     ]";
static const char *TAG_SPIFFS              = "[SPIFFS    ]";

UIHandler uiHandler(PIN_SDA, PIN_SCL);
radioHandler sxradio(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS, LORA_DIO0, LORA_DIO1, LORA_DIO2, LORA_RST);

esp_vfs_spiffs_conf_t config = {
    .base_path              = "/spiffs",
    .partition_label        = NULL,
    .max_files              = 5,
    .format_if_mount_failed = true,
};

// Void to read the config, store the CALLSIGN value and the ADDRESSES array in a passed variable
void readConfig(char *CALLSIGN, int (*ADDRESSES)[2]) {
    // esp_vfs_spiffs_register(&config);
    cJSON *root       = NULL;
    cJSON *callsign   = NULL;
    cJSON *addresses  = NULL;
    char *fileContent = NULL;
    FILE *f           = fopen("/spiffs/config.json", "r");
    if (f == NULL) {
        ESP_LOGE(TAG_SPIFFS, "Failed to open file for reading");
    } else {
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);
        fileContent = (char *)malloc(fsize + 1);
        fread(fileContent, 1, fsize, f);
        fclose(f);
        fileContent[fsize] = 0;
        root               = cJSON_Parse(fileContent);
        if (root == NULL) {
            ESP_LOGE(TAG_SPIFFS, "Error before: [%s]\n", cJSON_GetErrorPtr());
        } else {
            callsign = cJSON_GetObjectItem(root, "CALLSIGN");
            if (callsign == NULL) {
                ESP_LOGE(TAG_SPIFFS, "CALLSIGN not found in config.json");
            } else {
                ESP_LOGI(TAG_SPIFFS, "CALLSIGN: %s", callsign->valuestring);
                strcpy(CALLSIGN, callsign->valuestring);
            }
            addresses = cJSON_GetObjectItem(root, "ADDRESSES");
            if (addresses == NULL) {
                ESP_LOGE(TAG_SPIFFS, "ADDRESSES not found in config.json");
            } else {
                cJSON *address = NULL;
                int i          = 0;
                cJSON_ArrayForEach(address, addresses) {
                    cJSON *address1 = cJSON_GetArrayItem(address, 0);
                    cJSON *address2 = cJSON_GetArrayItem(address, 1);
                    ESP_LOGI(TAG_SPIFFS, "Address: %d, %d", address1->valueint, address2->valueint);
                    ADDRESSES[i][0] = address1->valueint;
                    ADDRESSES[i][1] = address2->valueint;
                    i++;
                }
            }
        }
    }
    // esp_vfs_spiffs_unregister(NULL);
}

void dumpMessages() {
    // esp_vfs_spiffs_register(&config);
    ESP_LOGI(TAG_SPIFFS, "Reading messages.csv");
    FILE *f = fopen("/spiffs/messages.csv", "r");
    if (f == NULL) {
        ESP_LOGE(TAG_SPIFFS, "Failed to open file for reading");
    } else {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            ESP_LOGI(TAG_SPIFFS, "%s", line);
        }
        fclose(f);
    }
    // esp_vfs_spiffs_unregister(NULL);
}

void vUITask(void *pvParameters) {
    uiHandler.splashScreen();

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    uiHandler.showMenu(0);
    int currMenu = uiHandler.getMenu();
    int prevMenu = currMenu;
    for (;;) {
        currMenu = uiHandler.getMenu();
        if (currMenu != prevMenu) {
            uiHandler.redraw();
            prevMenu = currMenu;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void vRadioTask(void *pvParameters) {
    ESP_LOGI(TAG_RADIO, "Radio is running");
    ESP_LOGI(TAG_RADIO, "Radio is running on core %d\n", xPortGetCoreID());

    // sxradio.pocsagSendText(100, "TEST");

    sxradio.pocsagStartRx();

    char *message     = new char[32];
    int *address      = new int;
    char *fullMessage = new char[32];
    for (;;) {
        if (sxradio.pocsagAvailable() >= 2) {
            sxradio.pocsagGetMessage(address, message);
            int RSSI = sxradio.getRSSI();
            ESP_LOGI(TAG_RADIO, "RSSI : %d", RSSI);
            uiHandler.setRSSI(RSSI);

            // Check if address is in the list of addresses
            for (int i = 0; i < 10; i++) {
                if (ADDRESSES[i][0] == *address) {
                    ESP_LOGI(TAG_RADIO, "Address found in list");
                    // Concatenate the address and the message
                    sprintf(fullMessage, "%d: %s", *address, message);
                    uiHandler.displayMessage(fullMessage);
                    uiHandler.setNewMessage(true);

                    // Save to messages.csv with Address, message, RSSI
                    ESP_LOGI(TAG_SPIFFS, "Writing to messages.csv");
                    FILE *f = fopen("/spiffs/messages.csv", "a");
                    if (f == NULL) {
                        ESP_LOGE(TAG_SPIFFS, "Failed to open file for writing");
                    } else {
                        fprintf(f, "%d,%s,%d\n", *address, message, RSSI);
                        fclose(f);
                    }
                    // esp_vfs_spiffs_unregister(NULL);

                    break;
                }
            }

            sxradio.pocsagStartRx();
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

static void IRAM_ATTR gpio_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t)arg;
    if (gpio_num == UP) {
        uiHandler.upButton(NULL);
    } else if (gpio_num == OK) {
        uiHandler.okButton(NULL);
    } else if (gpio_num == DOWN) {
        uiHandler.downButton(NULL);
    }

}

void app_main() {
    esp_vfs_spiffs_register(&config);
    readConfig(CALLSIGN, ADDRESSES);
    dumpMessages();

    uiHandler.init(CALLSIGN, TAG_UI, ESP_LOG_WARN);
    uiHandler.splashScreen();

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(UP, GPIO_MODE_INPUT);
    gpio_set_direction(OK, GPIO_MODE_INPUT);
    gpio_set_direction(DOWN, GPIO_MODE_INPUT);

    gpio_pullup_en(UP);
    gpio_pulldown_dis(UP);
    gpio_pullup_en(OK);
    gpio_pulldown_dis(OK);
    gpio_pullup_en(DOWN);
    gpio_pulldown_dis(DOWN);

    gpio_set_intr_type(UP, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(OK, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(DOWN, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service((int)ESP_INTR_FLAG_IRAM);

    gpio_isr_handler_add(UP, gpio_isr_handler, (void *)UP);
    gpio_isr_handler_add(OK, gpio_isr_handler, (void *)OK);
    gpio_isr_handler_add(DOWN, gpio_isr_handler, (void *)DOWN);

    sxradio.pocsagInit(frequency, offset, TAG_RADIO, ESP_LOG_INFO);

    esp_log_level_set(TAG_MAIN, ESP_LOG_WARN);
    esp_log_level_set(TAG_LED, ESP_LOG_WARN);
    esp_log_level_set(TAG_SPIFFS, ESP_LOG_INFO);

    xTaskCreate(vRadioTask,
                "RadioTask",
                50000,
                NULL,
                RadioTaskPriority,
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

    for (;;) {
        // Display the core on which the main function is running
        ESP_LOGI(TAG_MAIN, "app_main() is running on core %d\n", xPortGetCoreID());
        // Wait 1 seconds
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
