#include "AHT20.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

float AHT20_Temperature = 0.0;
float AHT20_Humidity = 0.0;

uint8_t AHT20_Read_Status(void)
{
    uint8_t Byte_first;
    HAL_I2C_Master_Receive(&hi2c1, AHT20_IIC_ADDR, &Byte_first, 1, AHT20_IIC_BUS_WAIT);
    return Byte_first;
}

uint8_t AHT20_Read_Cal_Enable(void)
{
    uint8_t val = 0;
    val = AHT20_Read_Status();
    if ((val & 0x68) == 0x08)
        return 1;
    else
        return 0;
}

void Aht20_Reset(void)
{
    uint8_t cmd_buf = 0XBA;
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_IIC_ADDR, &cmd_buf, 1, AHT20_IIC_BUS_WAIT);
}

void AHT20_SendAC(void)
{
    uint8_t cmd_buf[3] = {0xac, 0x33, 0x00};
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_IIC_ADDR, cmd_buf, 3, AHT20_IIC_BUS_WAIT);
}

void AHT20_Read_CTdata(uint32_t *ct)
{
    volatile uint8_t Byte_1th = 0;
    volatile uint8_t Byte_2th = 0;
    volatile uint8_t Byte_3th = 0;
    volatile uint8_t Byte_4th = 0;
    volatile uint8_t Byte_5th = 0;
    volatile uint8_t Byte_6th = 0;

    uint8_t read_buf[6];
    uint32_t RetuData = 0;
    uint16_t cnt = 0;

    AHT20_SendAC();
    HAL_Delay(80);

    cnt = 0;
    while (((AHT20_Read_Status() & 0x80) == 0x80))
    {
        HAL_Delay(2);
        if (cnt++ >= 100)
        {
            break;
        }
    }

    HAL_I2C_Master_Receive(&hi2c1, AHT20_IIC_ADDR, read_buf, 6, AHT20_IIC_BUS_WAIT);

    Byte_1th = read_buf[0];
    Byte_2th = read_buf[1];
    Byte_3th = read_buf[2];
    Byte_4th = read_buf[3];
    Byte_5th = read_buf[4];
    Byte_6th = read_buf[5];

    RetuData = (RetuData | Byte_2th) << 8;
    RetuData = (RetuData | Byte_3th) << 8;
    RetuData = (RetuData | Byte_4th);
    RetuData = RetuData >> 4;
    ct[0] = RetuData;

    RetuData = 0;
    RetuData = (RetuData | Byte_4th) << 8;
    RetuData = (RetuData | Byte_5th) << 8;
    RetuData = (RetuData | Byte_6th);
    RetuData = RetuData & 0xfffff;
    ct[1] = RetuData;
}

void AHT20_Init(void)
{
    uint8_t cmd_buf[3];
    HAL_StatusTypeDef ret;

    cmd_buf[0] = 0xa8;
    cmd_buf[1] = 0x00;
    cmd_buf[2] = 0x00;
    ret = HAL_I2C_Master_Transmit(&hi2c1, AHT20_IIC_ADDR, cmd_buf, 3, 100);
    if (ret != HAL_OK) {
        return; // 初始化失败，直接返回
    }
    HAL_Delay(10);

    cmd_buf[0] = 0xbe;
    cmd_buf[1] = 0x08;
    cmd_buf[2] = 0x00;
    ret = HAL_I2C_Master_Transmit(&hi2c1, AHT20_IIC_ADDR, cmd_buf, 3, 100);
    if (ret != HAL_OK) {
        return; // 初始化失败，直接返回
    }
    HAL_Delay(10);
}

void AHT20_Read_Data(void)
{
    uint32_t CT_data[2] = {0, 0};
    AHT20_Read_CTdata(CT_data);
    AHT20_Humidity = (float)CT_data[0] * 100 / 1024 / 1024;
    AHT20_Temperature = (float)CT_data[1] * 200 / 1024 / 1024 - 50;
}