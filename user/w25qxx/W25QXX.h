#ifndef __W25QXX_H
#define __W25QXX_H

#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef hspi2;

#define W25QXX_CS_PIN GPIO_PIN_12
#define W25QXX_CS_PORT GPIOB

void W25QXX_Init(void);
void W25QXX_ReadID(uint8_t *MID, uint16_t *DID);
void W25QXX_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count);
void W25QXX_SectorErase(uint32_t Address);
void W25QXX_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count);

#endif