#include <stdio.h>
#include "sdkconfig.h"
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "driver/gpio.h"
#include <driver/spi_master.h>
#include <driver/i2c_master.h>
#include "u8g2.h"
#include "u8g2_esp32_hal.h"
#include "graphics.h"

// SDA - GPIO21
#define PIN_SDA 21
// SCL - GPIO22
#define PIN_SCL 22


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

static const char *TAG = "ssd1306";

void task_test_SSD1306i2c(void *ignore) {
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.bus.i2c.sda = PIN_SDA;
    u8g2_esp32_hal.bus.i2c.scl = PIN_SCL;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_t u8g2;  // a structure which will contain all the data for one display
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2, U8G2_R0,
       // u8x8_byte_sw_i2c,
        u8g2_esp32_i2c_byte_cb,
      u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
    u8x8_SetI2CAddress(&u8g2.u8x8, 0x3c);

    ESP_LOGI(TAG, "u8g2_InitDisplay");
    u8g2_InitDisplay(&u8g2);  // send init sequence to the display, display is in
                            // sleep mode after this,

    ESP_LOGI(TAG, "u8g2_SetPowerSave");
    u8g2_SetPowerSave(&u8g2, 0);  // wake up display
    ESP_LOGI(TAG, "u8g2_ClearBuffer");
    u8g2_ClearBuffer(&u8g2);
    // ESP_LOGI(TAG, "u8g2_DrawBox");
    // u8g2_DrawBox(&u8g2, 0, 26, 80, 6);
    // u8g2_DrawFrame(&u8g2, 0, 26, 100, 6);

    ESP_LOGI(TAG, "u8g2_DrawBitmap");
    u8g2_DrawBitmap(&u8g2, 0, 40, 128/8, 20, fredcorp_logo);

    ESP_LOGI(TAG, "u8g2_SetFont");
    u8g2_SetFont(&u8g2, u8g2_font_pxplusibmvga9_t_all);
    ESP_LOGI(TAG, "u8g2_DrawStr");
    u8g2_DrawStr(&u8g2, 18, 30, "fredcorp.cc");

    ESP_LOGI(TAG, "u8g2_SendBuffer");
    u8g2_SendBuffer(&u8g2);

    ESP_LOGI(TAG, "All done!");

    vTaskDelete(NULL);
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
                1000,        // The number of words to allocate for use as the task's stack (arbitrary size enough for this task)
                NULL,         // No parameter passed to the task
                taskPriority, // Priority of the task
                NULL);        // No handle
    xTaskCreate(vTask2,       // Entry function of the task
                "Task2",      // Name of the task
                1000,        // The number of words to allocate for use as the task's stack (arbitrary size enough for this task)
                NULL,         // No parameter passed to the task
                taskPriority, // Priority of the task
                NULL);        // No handle
    xTaskCreate(task_test_SSD1306i2c,
                "oled",
                20000,
                NULL,
                taskPriority,
                NULL);
    for (;;)
    {
        // Display the core on which the main function is running
        printf("app_main() is running on core %d\n", xPortGetCoreID());
        // Wait 1 seconds
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}