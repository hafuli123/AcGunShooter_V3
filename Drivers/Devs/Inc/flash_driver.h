#ifndef __FLASH_DRIVER_H__
#define __FLASH_DRIVER_H__

#include <rtthread.h>
#include "stm32f4xx_hal.h"

#include "spi.h"

#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256
#define W25X_WriteEnable                    0x06
#define W25X_WriteDisable                   0x04
#define W25X_ReadStatusReg                0x05
#define W25X_WriteStatusReg               0x01
#define W25X_ReadData                         0x03
#define W25X_FastReadData                   0x0B
#define W25X_FastReadDual                   0x3B
#define W25X_PageProgram                    0x02
#define W25X_BlockErase                     0xD8
#define W25X_SectorErase                    0x20
#define W25X_ChipErase                      0xC7
#define W25X_PowerDown                      0xB9
#define W25X_ReleasePowerDown             0xAB
#define W25X_DeviceID                         0xAB
#define W25X_ManufactDeviceID             0x90
#define W25X_JedecDeviceID                0x9F

#define WIP_Flag                        0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                      0xFF

#define  SPI_FLASH_ID                       0X4018     //W25Q128  16MB

#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(G, FLASH_SPI_CS_PIN, GPIO_PIN_RESET)
#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(FLASH_SPI_GPIO_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET)

#define FLASH_SPI_MISO_PORT                        GPIOA
#define FLASH_SPI_MISO_PIN                         GPIO_PIN_6

void flash_init(uint8_t _flash_spi);
void flash_bufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void flash_sectorErase(uint32_t SectorAddr);
void flash_bufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
#endif
