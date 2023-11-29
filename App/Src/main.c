#include "main.h"
#include "oled_driver.h"
#include "flash_driver.h"

int main(void)
{
	MX_I2C1_Init();
    oled_init();
    oled_clear();  
	flash_init(SEL_SPI1);
	
    oled_drawBMP(0, 0, 120, 8, oled_bmp_bootScr);	
	
//	uint8_t tx[6] ={1,2,3,4,5,6};
//	flash_sectorErase(0x03000);
//	flash_bufferWrite(tx, 0x03000, 6);
	uint8_t rx[10];
	flash_bufferRead(rx, 0x03000, 10);
	while(1)
	{
	}
	return RT_EOK;
}
