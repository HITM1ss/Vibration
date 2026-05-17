#ifndef _AHT20_H_
#define _AHT20_H_

#include "stm32f1xx_hal.h"

#define AHT20_IIC_ADDR 0x70
#define AHT20_IIC_BUS_WAIT 0XFFFFFF

extern float AHT20_Temperature;
extern float AHT20_Humidity;

uint8_t AHT20_Read_Status(void);
uint8_t AHT20_Read_Cal_Enable(void);
void AHT20_Init(void);
void AHT20_SendAC(void);
void AHT20_Read_CTdata(uint32_t *ct);
void Aht20_Reset(void);
void AHT20_Read_Data(void);

#endif