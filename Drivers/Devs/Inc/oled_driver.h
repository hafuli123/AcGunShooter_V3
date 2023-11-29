#ifndef __OLED_DRIVER_H__
#define __OLED_DRIVER_H__

#include <rtthread.h>
#include "stm32f4xx_hal.h"

#include "i2c.h"
#include "oled_discode.h"

#define	OLED_DEV_ADDR	0x78

void oled_on(void);
void oled_off(void);

void oled_init(void);
void oled_showStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize);
void oled_showCN(uint8_t x, uint8_t y, uint8_t n, uint8_t * words) ;
void oled_drawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[]);
void oled_clear(void);

#endif
