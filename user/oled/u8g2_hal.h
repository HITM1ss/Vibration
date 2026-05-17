#ifndef U8G2_HAL_H
#define U8G2_HAL_H

#include "u8g2.h"
#include "u8x8.h"

uint8_t u8x8_gpio_and_delay_stm32f1(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif /* U8G2_HAL_H */