#include "UIHandler.h"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <driver/i2c_master.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <stdio.h>

#define CALLSIGN "ON4PFD"

// SDA - GPIO21
#define PIN_SDA 21
// SCL - GPIO22
#define PIN_SCL 22

#define LED GPIO_NUM_25

extern "C" {
void app_main(void);
}

static bool ledState = false;

static const UBaseType_t UITaskPriority = 2;
static const UBaseType_t taskPriority = 1;

static const char *TAG_MAIN = "MAIN";
static const char *TAG_SCREEN = "SCREEN";
static const char *TAG_TASK1 = "TASK1";
static const char *TAG_TASK2 = "TASK2";

const char *model_info(esp_chip_model_t model) {
    switch (model) {
    case CHIP_ESP32:
        return "ESP32";
    case CHIP_ESP32S2:
        return "ESP32S2";
    case CHIP_ESP32S3:
        return "ESP32S3";
    case CHIP_ESP32C3:
        return "ESP32C3";
    case CHIP_ESP32H2:
        return "ESP32H2";
    case CHIP_ESP32C2:
        return "ESP32C2";
    default:
        return "Unknown";
    }
}

void print_chip_info() {
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG_TASK1, "Chip model %s with %d CPU core(s), WiFi%s%s, ",
             model_info(chip_info.model), chip_info.cores,
             (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
             (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    ESP_LOGI(TAG_TASK1, "silicon revision v%d.%d\n", major_rev, minor_rev);
}

void vUITask(void *pvParameters) {
    UIHandler uiHandler(CALLSIGN);
    uiHandler.init(PIN_SDA, PIN_SCL, "UI", ESP_LOG_INFO);
    uiHandler.splashScreen();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    for (;;) {
        uiHandler.setRSSI(0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        uiHandler.setRSSI(12);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        uiHandler.setRSSI(42);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        uiHandler.setRSSI(69);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        uiHandler.setRSSI(100);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        for (int i = 0; i <= 3; i++) {
            uiHandler.showMenu(i);
            vTaskDelay(1500 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}

void vTask1(void *pvParameters) {
    for (;;) {
        ESP_LOGI(TAG_TASK1, "Task1 is running");
        ESP_LOGI(TAG_TASK1, "Task1 is running on core %d\n", xPortGetCoreID());
        print_chip_info();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void vTask2(void *pvParameters) {
    for (;;) {
        ESP_LOGI(TAG_TASK2, "Task2 is running");
        ESP_LOGI(TAG_TASK2, "Task2 is running on core %d\n", xPortGetCoreID());
        ledState = !ledState;
        gpio_set_level(LED, ledState);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    esp_log_level_set(TAG_MAIN, ESP_LOG_INFO);
    esp_log_level_set(TAG_SCREEN, ESP_LOG_INFO);
    esp_log_level_set(TAG_TASK1, ESP_LOG_INFO);
    esp_log_level_set(TAG_TASK2, ESP_LOG_INFO);

    xTaskCreate(vTask1,
                "Task1",
                5000,
                NULL,
                taskPriority,
                NULL);
    xTaskCreate(vTask2,         // Entry function of the task
                "Task2",        // Name of the task
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
