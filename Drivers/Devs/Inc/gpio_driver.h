#ifndef __GPIO_DRIVER_H__
#define __GPIO_DRIVER_H__

#include <rtthread.h>
#include "stm32f4xx_hal.h"

#include "gpio.h"

#define PWR_OFF	0
#define PWR_ON	1


uint8_t bat_getChargeStatus(void);
void pwr_supplyCtrl(uint8_t _ctrl, uint16_t _time);

#endif
