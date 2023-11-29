#include "gpio_driver.h"

/*
 * ������: bat_getChargeStatus
 * ����: ��س��״̬�Ĳ�ѯ��
 * ����ֵ: ���״̬
 */
uint8_t bat_getChargeStatus(void)
{
	uint8_t ret;
	 
	//������Ƿ���� | �������Ƿ������
	ret = ((uint8_t)HAL_GPIO_ReadPin(BAT_CHGCMPLT_I_GPIO_Port, BAT_CHGCMPLT_I_Pin) << 4)
		| (uint8_t)HAL_GPIO_ReadPin(BAT_CHGSTAT_I_GPIO_Port, BAT_CHGSTAT_I_Pin) ;
	//ret��ֵ��0x01:����+����	0x11��δ����+����	0x?0��δ����
	return ret;
}
 
 /*
 * ������: pwr_supplyCtrl
 * ����: ��Դ������ơ�
 * ������_ctrl��0�ص� 1ͨ�磻_time���ϵ�ʱ����ť�����ʱ����
 * ����ֵ: 0
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
 

