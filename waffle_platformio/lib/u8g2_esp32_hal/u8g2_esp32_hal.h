/*
 * from https://gitlab.com/HashaVK/u8g2-hal-esp-idf
 */

#pragma once

#include "u8g2.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"

#define U8G2_ESP32_HAL_UNDEFINED	GPIO_NUM_NC

#if SOC_I2C_NUM > 1
#define I2C_MASTER_NUM				I2C_NUM_1 // I2C port number for master dev
#else
#define I2C_MASTER_NUM				I2C_NUM_0 // I2C port number for master dev
#endif

#define I2C_MASTER_TX_BUF_DISABLE	0 // I2C master do not need buffer
#define I2C_MASTER_RX_BUF_DISABLE	0 // I2C master do not need buffer
#define ACK_CHECK_EN				0x01 // I2C master will check ack from slave
#define ACK_CHECK_DIS				0x00 // I2C master will not check ack from slave

#ifdef __cplusplus
extern "C" {
#endif

/** @public
 * HAL configuration structure.
 */
typedef struct
{
	union
	{
		/* SPI settings. */
		struct
		{
			/* GPIO num for clock. */
			gpio_num_t clk;
			/* GPIO num for SPI mosi. */
			gpio_num_t mosi;
			/* GPIO num for SPI slave/chip select. */
			gpio_num_t cs;
		} spi;
		/* I2C settings. */
		struct
		{
			/* GPIO num for I2C data. */
			gpio_num_t sda;
			/* GPIO num for I2C clock. */
			gpio_num_t scl;
		} i2c;
	} bus;
	/* Bus clock speed */
	uint32_t clk_speed;
	/* GPIO num for reset. */
	gpio_num_t reset;
	/* GPIO num for DC. */
	gpio_num_t dc;
} u8g2_esp32_hal_t;

/**
 * Construct a default HAL configuration with all fields undefined.
 */
#define U8G2_ESP32_HAL_DEFAULT						\
	{												\
		.bus =										\
		{											\
			.spi =									\
			{										\
				.clk = U8G2_ESP32_HAL_UNDEFINED,	\
				.mosi = U8G2_ESP32_HAL_UNDEFINED,	\
				.cs = U8G2_ESP32_HAL_UNDEFINED		\
			},										\
		},											\
		.clk_speed = 50000,							\
		.reset = U8G2_ESP32_HAL_UNDEFINED,			\
		.dc = U8G2_ESP32_HAL_UNDEFINED				\
	}

/**
 * Initialize the HAL with the given configuration.
 *
 * @see u8g2_esp32_hal_t
 * @see U8G2_ESP32_HAL_DEFAULT
 */
void u8g2_esp32_hal_init(u8g2_esp32_hal_t u8g2_esp32_hal_param);
uint8_t u8g2_esp32_spi_byte_cb(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);

#ifdef __cplusplus
}
#endif
