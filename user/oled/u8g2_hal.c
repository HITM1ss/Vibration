#include "u8g2_hal.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

#define OLED_CS_PIN    GPIO_PIN_4
#define OLED_CS_PORT   GPIOA
#define OLED_DC_PIN    GPIO_PIN_6
#define OLED_DC_PORT   GPIOA
#define OLED_RST_PIN   GPIO_PIN_0
#define OLED_RST_PORT  GPIOB
#define OLED_SCK_PIN   GPIO_PIN_5
#define OLED_SCK_PORT  GPIOA
#define OLED_SDA_PIN   GPIO_PIN_7
#define OLED_SDA_PORT  GPIOA

uint8_t u8x8_gpio_and_delay_stm32f1(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            break;
        case U8X8_MSG_DELAY_MILLI:
            HAL_Delay(arg_int);
            break;
        case U8X8_MSG_DELAY_100NANO:
            __NOP();
            break;
        case U8X8_MSG_DELAY_10MICRO:
            for(int i = 0; i < arg_int; i++) {
                for(volatile int j = 0; j < 120; j++) __NOP();
            }
            break;
        case U8X8_MSG_DELAY_I2C:
            for(volatile int i = 0; i < arg_int * 10; i++) __NOP();
            break;
        case U8X8_MSG_GPIO_CS:
            HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_PIN, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_DC:
            HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_PIN, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_RESET:
            HAL_GPIO_WritePin(OLED_RST_PORT, OLED_RST_PIN, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_SPI_CLOCK:
            HAL_GPIO_WritePin(OLED_SCK_PORT, OLED_SCK_PIN, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        case U8X8_MSG_GPIO_SPI_DATA:
            HAL_GPIO_WritePin(OLED_SDA_PORT, OLED_SDA_PIN, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;
        default:
            return 0;
    }
    return 1;
}

uint8_t u8x8_byte_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[32];
    static uint8_t buf_idx;
    uint8_t *data;

    switch(msg)
    {
        case U8X8_MSG_BYTE_SEND:
            HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)arg_ptr, arg_int);
            while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
            break;
        case U8X8_MSG_BYTE_INIT:
            break;
        case U8X8_MSG_BYTE_SET_DC:
            u8x8_gpio_and_delay_stm32f1(u8x8, U8X8_MSG_GPIO_DC, arg_int, NULL);
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            u8x8_gpio_and_delay_stm32f1(u8x8, U8X8_MSG_GPIO_CS, 0, NULL);
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_10MICRO, 1, NULL);
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_10MICRO, 1, NULL);
            u8x8_gpio_and_delay_stm32f1(u8x8, U8X8_MSG_GPIO_CS, 1, NULL);
            break;
        default:
            return 0;
    }
    return 1;
}