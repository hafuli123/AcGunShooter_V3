#include "main.h"
#include "oled_driver.h"

int main(void)
{
	MX_I2C1_Init();
    OLED_Init();
    OLED_Clear();
	
    OLED_DrawBMP(0, 0, 120, 8, oled_bmp_bootScr);	
	
	while(1)
	{
	}
	return RT_EOK;
}
