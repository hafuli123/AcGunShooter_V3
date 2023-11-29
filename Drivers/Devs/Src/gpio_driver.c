#include "gpio_driver.h"

/*
 * 函数名: bat_getChargeStatus
 * 描述: 电池充电状态的查询。
 * 返回值: 充电状态
 */
uint8_t bat_getChargeStatus(void)
{
	uint8_t ret;
	 
	//检测充电是否充满 | 检测充电口是否插了线
	ret = ((uint8_t)HAL_GPIO_ReadPin(BAT_CHGCMPLT_I_GPIO_Port, BAT_CHGCMPLT_I_Pin) << 4)
		| (uint8_t)HAL_GPIO_ReadPin(BAT_CHGSTAT_I_GPIO_Port, BAT_CHGSTAT_I_Pin) ;
	//ret的值：0x01:充满+插线	0x11：未充满+插线	0x?0：未插线
	return ret;
}
 
 /*
 * 函数名: pwr_supplyCtrl
 * 描述: 电源供电控制。
 * 参数：_ctrl：0关电 1通电；_time：上电时按按钮多久延时开机
 * 返回值: 0
 */
void pwr_supplyCtrl(uint8_t _ctrl, uint16_t _time)
{
	if(_ctrl == PWR_OFF)
	{
		HAL_GPIO_WritePin(PWR_CTRL_O_GPIO_Port, PWR_CTRL_O_Pin, GPIO_PIN_RESET);
		return;
	}
	else
	{
		rt_thread_mdelay(_time);
		HAL_GPIO_WritePin(PWR_CTRL_O_GPIO_Port, PWR_CTRL_O_Pin, GPIO_PIN_SET);
        return;	
	}
}
 

