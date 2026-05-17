#include "W25QXX.h"
#include "W25QXX_Ins.h"

void W25QXX_Init(void)
{
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET);
}

void W25QXX_ReadID(uint8_t *MID, uint16_t *DID)
{
	uint8_t tx_data[1] = {W25QXX_JEDEC_ID};
	uint8_t rx_data[3];
	
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, tx_data, 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi2, rx_data, 3, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET);
	
	*MID = rx_data[0];
	*DID = (rx_data[1] << 8) | rx_data[2];
}

void W25QXX_WriteEnable(void)
{
	uint8_t tx_data[1] = {W25QXX_WRITE_ENABLE};
	
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, tx_data, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET);
}

void W25QXX_WaitBusy(void)
{
	uint8_t tx_data[1] = {W25QXX_READ_STATUS_REGISTER_1};
	uint8_t rx_data;
	uint32_t Timeout = 100000;
	
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, tx_data, 1, HAL_MAX_DELAY);
	
	while (Timeout > 0)
	{
		HAL_SPI_Receive(&hspi2, &rx_data, 1, HAL_MAX_DELAY);
		if ((rx_data & 0x01) == 0x00)
		{
			break;
		}
		Timeout--;
	}
	
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET);
}

void W25QXX_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
	uint8_t tx_data[4];
	
	W25QXX_WriteEnable();
	
	tx_data[0] = W25QXX_PAGE_PROGRAM;
	tx_data[1] = Address >> 16;
	tx_data[2] = Address >> 8;
	tx_data[3] = Address;
	
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, tx_data, 4, HAL_MAX_DELAY);
	HAL_SPI_Transmit(&hspi2, DataArray, Count, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET);
	
	W25QXX_WaitBusy();
}

void W25QXX_SectorErase(uint32_t Address)
{
	uint8_t tx_data[4];
	
	W25QXX_WriteEnable();
	
	tx_data[0] = W25QXX_SECTOR_ERASE_4KB;
	tx_data[1] = Address >> 16;
	tx_data[2] = Address >> 8;
	tx_data[3] = Address;
	
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, tx_data, 4, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET);
	
	W25QXX_WaitBusy();
}

void W25QXX_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint8_t tx_data[4];
	
	tx_data[0] = W25QXX_READ_DATA;
	tx_data[1] = Address >> 16;
	tx_data[2] = Address >> 8;
	tx_data[3] = Address;
	
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, tx_data, 4, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi2, DataArray, Count, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET);
}