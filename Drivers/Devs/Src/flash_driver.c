#include "flash_driver.h"

SPI_HandleTypeDef hspiflash;

static void flash_bulkErase(void);
static void flash_pageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
static uint32_t flash_readID(void);
static uint32_t flash_readDeviceID(void);
static void flash_startReadSequence(uint32_t ReadAddr);
static void flash_powerDown(void);
static void flash_wakeUp(void);
static uint8_t flash_readByte(void);
static uint8_t flash_sendByte(uint8_t byte);
static void flash_writeEnable(void);
static void flash_waitForWriteEnd(void);

void flash_init(uint8_t _flash_spi)
{
	if(_flash_spi == SEL_SPI1)
	{
		MX_SPI1_Init();
		*(SPI_HandleTypeDef*)&hspiflash = *(SPI_HandleTypeDef*)&hspi1;	//指针有风险，以后考虑不采用这种方式
	}
	
}

//void flash_read(uint32_t _addr)
//{
//	flash_bufferRead()
//}

static void flash_spiCS_enable(void)
{
	HAL_GPIO_WritePin(SPI1_CS_PORT, SPI1_CS_PIN, GPIO_PIN_RESET);
}
static void flash_spiCS_disable(void)
{
	HAL_GPIO_WritePin(SPI1_CS_PORT, SPI1_CS_PIN, GPIO_PIN_SET);
}

/**
  * 函数功能: 擦除扇区
  * 输入参数: SectorAddr：待擦除扇区地址，要求为4096倍数
  * 返 回 值: 无
  * 说    明：串行Flash最小擦除块大小为4KB(4096字节)，即一个扇区大小，要求输入参数
  *           为4096倍数。在往串行Flash芯片写入数据之前要求先擦除空间。
  */
void flash_sectorErase(uint32_t SectorAddr)
{
  /* 发送FLASH写使能命令 */
  flash_writeEnable();
  flash_waitForWriteEnd();
  /* 擦除扇区 */
  /* 选择串行FLASH: CS低电平 */
  flash_spiCS_enable();
  /* 发送扇区擦除指令*/
  flash_sendByte(W25X_SectorErase);
  /*发送擦除扇区地址的高位*/
  flash_sendByte((SectorAddr & 0xFF0000) >> 16);
  /* 发送擦除扇区地址的中位 */
  flash_sendByte((SectorAddr & 0xFF00) >> 8);
  /* 发送擦除扇区地址的低位 */
  flash_sendByte(SectorAddr & 0xFF);
  /* 禁用串行FLASH: CS 高电平 */
  flash_spiCS_disable();
  /* 等待擦除完毕*/
  flash_waitForWriteEnd();
}

/**
  * 函数功能: 擦除整片
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：擦除串行Flash整片空间
  */
static void flash_bulkErase(void)
{
 /* 发送FLASH写使能命令 */
  flash_writeEnable();

  /* 整片擦除 Erase */
  /* 选择串行FLASH: CS低电平 */
  flash_spiCS_enable();
  /* 发送整片擦除指令*/
  flash_sendByte(W25X_ChipErase);
  /* 禁用串行FLASH: CS高电平 */
  flash_spiCS_disable();

  /* 等待擦除完毕*/
  flash_waitForWriteEnd();
}

/**
  * 函数功能: 往串行FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
  * 输入参数: pBuffer：待写入数据的指针
  *           WriteAddr：写入地址
  *           NumByteToWrite：写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
  * 返 回 值: 无
  * 说    明：串行Flash每页大小为256个字节
  */
static void flash_pageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /* 发送FLASH写使能命令 */
  flash_writeEnable();

   /* 寻找串行FLASH: CS低电平 */
  flash_spiCS_enable();
  /* 写送写指令*/
  flash_sendByte(W25X_PageProgram);
  /*发送写地址的高位*/
  flash_sendByte((WriteAddr & 0xFF0000) >> 16);
  /*发送写地址的中位*/
  flash_sendByte((WriteAddr & 0xFF00) >> 8);
  /*发送写地址的低位*/
  flash_sendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("Err: flash_pageWrite too large!\n");
  }

  /* 写入数据*/
  while (NumByteToWrite--)
  {
     /* 发送当前要写入的字节数据 */
    flash_sendByte(*pBuffer);
     /* 指向下一字节数据 */
    pBuffer++;
  }

  /* 禁用串行FLASH: CS 高电平 */
  flash_spiCS_disable();

  /* 等待写入完毕*/
  flash_waitForWriteEnd();
}

/**
  * 函数功能: 往串行FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * 输入参数: pBuffer：待写入数据的指针
  *           WriteAddr：写入地址
  *           NumByteToWrite：写入数据长度
  * 返 回 值: 无
  * 说    明：该函数可以设置任意写入数据长度
  */
void flash_bufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* 若地址与 SPI_FLASH_PageSize 对齐  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      flash_pageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        flash_pageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      flash_pageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* 若地址与 SPI_FLASH_PageSize 不对齐 */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        flash_pageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        flash_pageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        flash_pageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      flash_pageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        flash_pageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        flash_pageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/**
  * 函数功能: 从串行Flash读取数据
  * 输入参数: pBuffer：存放读取到数据的指针
  *           ReadAddr：读取数据目标地址
  *           NumByteToRead：读取数据长度
  * 返 回 值: 无
  * 说    明：该函数可以设置任意读取数据长度
  */
void flash_bufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /* 选择串行FLASH: CS低电平 */
  flash_spiCS_enable();

  /* 发送 读 指令 */
  flash_sendByte(W25X_ReadData);

  /* 发送 读 地址高位 */
  flash_sendByte((ReadAddr & 0xFF0000) >> 16);
  /* 发送 读 地址中位 */
  flash_sendByte((ReadAddr& 0xFF00) >> 8);
  /* 发送 读 地址低位 */
  flash_sendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* 读取数据 */
  {
     /* 读取一个字节*/
    *pBuffer = flash_sendByte(Dummy_Byte);
    /* 指向下一个字节缓冲区 */
    pBuffer++;
  }

  /* 禁用串行FLASH: CS 高电平 */
  flash_spiCS_disable();
}

/**
  * 函数功能: 读取串行Flash型号的ID
  * 输入参数: 无
  * 返 回 值: uint32_t：串行Flash的型号ID
  * 说    明：  FLASH_ID      IC型号      存储空间大小
                0x3015      W25X16        2M byte
                0x4015      W25Q16        4M byte
                0X4017      W25Q64        8M byte
                0X4018      W25Q128       16M byte  (YS-F1Pro开发板默认配置)
  */
static uint32_t flash_readID(void)
{
  uint32_t Temp = 0, Temp1 = 0, Temp2 = 0;

  /* 选择串行FLASH: CS低电平 */
  flash_spiCS_enable();

  /* 发送命令：读取芯片型号ID */
  flash_sendByte(W25X_JedecDeviceID);

  /* 从串行Flash读取一个字节数据 */
  Temp1 = flash_sendByte(Dummy_Byte);

  /* 从串行Flash读取一个字节数据 */
  Temp1 = flash_sendByte(Dummy_Byte);

  /* 从串行Flash读取一个字节数据 */
  Temp2 = flash_sendByte(Dummy_Byte);

  /* 禁用串行Flash：CS高电平 */
  flash_spiCS_disable();
  
  Temp = Temp1 << 8 | Temp2;
  return Temp;
}

/**
  * 函数功能: 读取串行Flash设备ID
  * 输入参数: 无
  * 返 回 值: uint32_t：串行Flash的设备ID
  * 说    明：
  */
static uint32_t flash_readDeviceID(void)
{
  uint32_t Temp = 0;

  /* 选择串行FLASH: CS低电平 */
  flash_spiCS_enable();

  /* 发送命令：读取芯片设备ID * */
  flash_sendByte(W25X_DeviceID);
  flash_sendByte(Dummy_Byte);
  flash_sendByte(Dummy_Byte);
  flash_sendByte(Dummy_Byte);
  
  /* 从串行Flash读取一个字节数据 */
  Temp = flash_sendByte(Dummy_Byte);

  /* 禁用串行Flash：CS高电平 */
  flash_spiCS_disable();

  return Temp;
}

/**
  * 函数功能: 启动连续读取数据串
  * 输入参数: ReadAddr：读取地址
  * 返 回 值: 无
  * 说    明：Initiates a read data byte (READ) sequence from the Flash.
  *           This is done by driving the /CS line low to select the device,
  *           then the READ instruction is transmitted followed by 3 bytes
  *           address. This function exit and keep the /CS line low, so the
  *           Flash still being selected. With this technique the whole
  *           content of the Flash is read with a single READ instruction.
  */
static void flash_startReadSequence(uint32_t ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  flash_spiCS_enable();

  /* Send "Read from Memory " instruction */
  flash_sendByte(W25X_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  flash_sendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  flash_sendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  flash_sendByte(ReadAddr & 0xFF);
}

/**
  * 函数功能: 从串行Flash读取一个字节数据
  * 输入参数: 无
  * 返 回 值: uint8_t：读取到的数据
  * 说    明：This function must be used only if the Start_Read_Sequence
  *           function has been previously called.
  */
static uint8_t flash_readByte(void)
{
  uint8_t d_read,d_send=Dummy_Byte;
  if(HAL_SPI_TransmitReceive(&hspiflash,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=Dummy_Byte;
  
  return d_read;    
}

/**
  * 函数功能: 往串行Flash读取写入一个字节数据并接收一个字节数据
  * 输入参数: byte：待发送数据
  * 返 回 值: uint8_t：接收到的数据
  * 说    明：无
  */
static uint8_t flash_sendByte(uint8_t byte)
{
  uint8_t d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(&hspiflash,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=Dummy_Byte;
  
  return d_read; 
}

/**
  * 函数功能: 使能串行Flash写操作
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void flash_writeEnable(void)
{
  /* 选择串行FLASH: CS低电平 */
  flash_spiCS_enable();

  /* 发送命令：写使能 */
  flash_sendByte(W25X_WriteEnable);

  /* 禁用串行Flash：CS高电平 */
  flash_spiCS_disable();
}

/**
  * 函数功能: 等待数据写入完成
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：Polls the status of the Write In Progress (WIP) flag in the
  *           FLASH's status  register  and  loop  until write  opertaion
  *           has completed.
  */
static void flash_waitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  flash_spiCS_enable();

  /* Send "Read Status Register" instruction */
  flash_sendByte(W25X_ReadStatusReg);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = flash_sendByte(Dummy_Byte);
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  flash_spiCS_disable();
}


/**
  * 函数功能: 进入掉电模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void flash_powerDown(void)   
{ 
  /* Select the FLASH: Chip Select low */
  flash_spiCS_enable();

  /* Send "Power Down" instruction */
  flash_sendByte(W25X_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  flash_spiCS_disable();
}   

/**
  * 函数功能: 唤醒串行Flash
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void flash_wakeUp(void)   
{
  /* Select the FLASH: Chip Select low */
  flash_spiCS_enable();

  /* Send "Power Down" instruction */
  flash_sendByte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  flash_spiCS_disable(); 
}   
