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
		*(SPI_HandleTypeDef*)&hspiflash = *(SPI_HandleTypeDef*)&hspi1;	//ָ���з��գ��Ժ��ǲ��������ַ�ʽ
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
  * ��������: ��������
  * �������: SectorAddr��������������ַ��Ҫ��Ϊ4096����
  * �� �� ֵ: ��
  * ˵    ��������Flash��С�������СΪ4KB(4096�ֽ�)����һ��������С��Ҫ���������
  *           Ϊ4096��������������FlashоƬд������֮ǰҪ���Ȳ����ռ䡣
  */
void flash_sectorErase(uint32_t SectorAddr)
{
  /* ����FLASHдʹ������ */
  flash_writeEnable();
  flash_waitForWriteEnd();
  /* �������� */
  /* ѡ����FLASH: CS�͵�ƽ */
  flash_spiCS_enable();
  /* ������������ָ��*/
  flash_sendByte(W25X_SectorErase);
  /*���Ͳ���������ַ�ĸ�λ*/
  flash_sendByte((SectorAddr & 0xFF0000) >> 16);
  /* ���Ͳ���������ַ����λ */
  flash_sendByte((SectorAddr & 0xFF00) >> 8);
  /* ���Ͳ���������ַ�ĵ�λ */
  flash_sendByte(SectorAddr & 0xFF);
  /* ���ô���FLASH: CS �ߵ�ƽ */
  flash_spiCS_disable();
  /* �ȴ��������*/
  flash_waitForWriteEnd();
}

/**
  * ��������: ������Ƭ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������������Flash��Ƭ�ռ�
  */
static void flash_bulkErase(void)
{
 /* ����FLASHдʹ������ */
  flash_writeEnable();

  /* ��Ƭ���� Erase */
  /* ѡ����FLASH: CS�͵�ƽ */
  flash_spiCS_enable();
  /* ������Ƭ����ָ��*/
  flash_sendByte(W25X_ChipErase);
  /* ���ô���FLASH: CS�ߵ�ƽ */
  flash_spiCS_disable();

  /* �ȴ��������*/
  flash_waitForWriteEnd();
}

/**
  * ��������: ������FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
  * �� �� ֵ: ��
  * ˵    ��������Flashÿҳ��СΪ256���ֽ�
  */
static void flash_pageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /* ����FLASHдʹ������ */
  flash_writeEnable();

   /* Ѱ�Ҵ���FLASH: CS�͵�ƽ */
  flash_spiCS_enable();
  /* д��дָ��*/
  flash_sendByte(W25X_PageProgram);
  /*����д��ַ�ĸ�λ*/
  flash_sendByte((WriteAddr & 0xFF0000) >> 16);
  /*����д��ַ����λ*/
  flash_sendByte((WriteAddr & 0xFF00) >> 8);
  /*����д��ַ�ĵ�λ*/
  flash_sendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("Err: flash_pageWrite too large!\n");
  }

  /* д������*/
  while (NumByteToWrite--)
  {
     /* ���͵�ǰҪд����ֽ����� */
    flash_sendByte(*pBuffer);
     /* ָ����һ�ֽ����� */
    pBuffer++;
  }

  /* ���ô���FLASH: CS �ߵ�ƽ */
  flash_spiCS_disable();

  /* �ȴ�д�����*/
  flash_waitForWriteEnd();
}

/**
  * ��������: ������FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú���������������д�����ݳ���
  */
void flash_bufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* ����ַ�� SPI_FLASH_PageSize ����  */
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
  else /* ����ַ�� SPI_FLASH_PageSize ������ */
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
  * ��������: �Ӵ���Flash��ȡ����
  * �������: pBuffer����Ŷ�ȡ�����ݵ�ָ��
  *           ReadAddr����ȡ����Ŀ���ַ
  *           NumByteToRead����ȡ���ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú����������������ȡ���ݳ���
  */
void flash_bufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /* ѡ����FLASH: CS�͵�ƽ */
  flash_spiCS_enable();

  /* ���� �� ָ�� */
  flash_sendByte(W25X_ReadData);

  /* ���� �� ��ַ��λ */
  flash_sendByte((ReadAddr & 0xFF0000) >> 16);
  /* ���� �� ��ַ��λ */
  flash_sendByte((ReadAddr& 0xFF00) >> 8);
  /* ���� �� ��ַ��λ */
  flash_sendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* ��ȡ���� */
  {
     /* ��ȡһ���ֽ�*/
    *pBuffer = flash_sendByte(Dummy_Byte);
    /* ָ����һ���ֽڻ����� */
    pBuffer++;
  }

  /* ���ô���FLASH: CS �ߵ�ƽ */
  flash_spiCS_disable();
}

/**
  * ��������: ��ȡ����Flash�ͺŵ�ID
  * �������: ��
  * �� �� ֵ: uint32_t������Flash���ͺ�ID
  * ˵    ����  FLASH_ID      IC�ͺ�      �洢�ռ��С
                0x3015      W25X16        2M byte
                0x4015      W25Q16        4M byte
                0X4017      W25Q64        8M byte
                0X4018      W25Q128       16M byte  (YS-F1Pro������Ĭ������)
  */
static uint32_t flash_readID(void)
{
  uint32_t Temp = 0, Temp1 = 0, Temp2 = 0;

  /* ѡ����FLASH: CS�͵�ƽ */
  flash_spiCS_enable();

  /* ���������ȡоƬ�ͺ�ID */
  flash_sendByte(W25X_JedecDeviceID);

  /* �Ӵ���Flash��ȡһ���ֽ����� */
  Temp1 = flash_sendByte(Dummy_Byte);

  /* �Ӵ���Flash��ȡһ���ֽ����� */
  Temp1 = flash_sendByte(Dummy_Byte);

  /* �Ӵ���Flash��ȡһ���ֽ����� */
  Temp2 = flash_sendByte(Dummy_Byte);

  /* ���ô���Flash��CS�ߵ�ƽ */
  flash_spiCS_disable();
  
  Temp = Temp1 << 8 | Temp2;
  return Temp;
}

/**
  * ��������: ��ȡ����Flash�豸ID
  * �������: ��
  * �� �� ֵ: uint32_t������Flash���豸ID
  * ˵    ����
  */
static uint32_t flash_readDeviceID(void)
{
  uint32_t Temp = 0;

  /* ѡ����FLASH: CS�͵�ƽ */
  flash_spiCS_enable();

  /* ���������ȡоƬ�豸ID * */
  flash_sendByte(W25X_DeviceID);
  flash_sendByte(Dummy_Byte);
  flash_sendByte(Dummy_Byte);
  flash_sendByte(Dummy_Byte);
  
  /* �Ӵ���Flash��ȡһ���ֽ����� */
  Temp = flash_sendByte(Dummy_Byte);

  /* ���ô���Flash��CS�ߵ�ƽ */
  flash_spiCS_disable();

  return Temp;
}

/**
  * ��������: ����������ȡ���ݴ�
  * �������: ReadAddr����ȡ��ַ
  * �� �� ֵ: ��
  * ˵    ����Initiates a read data byte (READ) sequence from the Flash.
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
  * ��������: �Ӵ���Flash��ȡһ���ֽ�����
  * �������: ��
  * �� �� ֵ: uint8_t����ȡ��������
  * ˵    ����This function must be used only if the Start_Read_Sequence
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
  * ��������: ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: byte������������
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
  */
static uint8_t flash_sendByte(uint8_t byte)
{
  uint8_t d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(&hspiflash,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=Dummy_Byte;
  
  return d_read; 
}

/**
  * ��������: ʹ�ܴ���Flashд����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void flash_writeEnable(void)
{
  /* ѡ����FLASH: CS�͵�ƽ */
  flash_spiCS_enable();

  /* �������дʹ�� */
  flash_sendByte(W25X_WriteEnable);

  /* ���ô���Flash��CS�ߵ�ƽ */
  flash_spiCS_disable();
}

/**
  * ��������: �ȴ�����д�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����Polls the status of the Write In Progress (WIP) flag in the
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
  * ��������: �������ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: ���Ѵ���Flash
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
