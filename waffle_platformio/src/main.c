#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "driver/gpio.h"

#define LED GPIO_NUM_25

static bool ledState = false;

const UBaseType_t taskPriority = 1;

const char *model_info(esp_chip_model_t model)
{
    switch (model)
    {
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

void print_chip_info()
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("Chip model %s with %d CPU core(s), WiFi%s%s, ",
            model_info(chip_info.model),
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d\n", major_rev, minor_rev);
}


void vTask1(void *pvParameters)
{
    for (;;)
    {
        // Display the core on which the task is running
        printf("Task1 is running on core %d\n", xPortGetCoreID());
        print_chip_info();
        // Wait 3 seconds
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}
void vTask2(void *pvParameters)
{
    for (;;)
    {
        printf("Task2 is running on core %d\n", xPortGetCoreID());
        ledState = !ledState;
        gpio_set_level(LED, ledState);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
void app_main()
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    xTaskCreate(vTask1,       // Entry function of the task
                "Task1",      // Name of the task
                10000,        // The number of words to allocate for use as the task's stack (arbitrary size enough for this task)
                NULL,         // No parameter passed to the task
                taskPriority, // Priority of the task
                NULL);        // No handle
    xTaskCreate(vTask2,       // Entry function of the task
                "Task2",      // Name of the task
                10000,        // The number of words to allocate for use as the task's stack (arbitrary size enough for this task)
                NULL,         // No parameter passed to the task
                taskPriority, // Priority of the task
                NULL);        // No handle
    for (;;)
    {
        // Display the core on which the main function is running
        printf("app_main() is running on core %d\n", xPortGetCoreID());
        // Wait 1 seconds
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}