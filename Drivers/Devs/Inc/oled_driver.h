#ifndef __OLED_DRIVER_H__
#define __OLED_DRIVER_H__

#include <rtthread.h>
#include "stm32f4xx_hal.h"

#include "i2c.h"
#include "oled_discode.h"

#define	OLED_DEV_ADDR	0x78

void OLED_ON(void);
void OLED_OFF(void);

void OLED_Init(void);
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize);
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t n, uint8_t * words) ;
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[]);
void OLED_Clear(void);

#endif
