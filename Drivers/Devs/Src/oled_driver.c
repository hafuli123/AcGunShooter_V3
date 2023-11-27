#include "oled_driver.h"

static void OLED_WriteCmd(uint8_t I2C_Command);
static void OLED_WriteDat(uint8_t I2C_Data);
static void OLED_SetPos(uint8_t x, uint8_t y);
static void OLED_Fill(uint8_t fill_Data);


/**
  * ��������: ��OLEDд������
  * �������: I2C_Command���������
  * �� �� ֵ: ��
  * ˵    ������
  */
static void OLED_WriteCmd(uint8_t I2C_Command)//д���� 
{
    I2C_Write(OLED_DEV_ADDR, 0x00, I2C_Command);
}

/**
  * ��������: ��OLEDд������
  * �������: I2C_Data������
  * �� �� ֵ: ��
  * ˵    ������
  */
static void OLED_WriteDat(uint8_t I2C_Data)//д����
{
    I2C_Write(OLED_DEV_ADDR, 0x40, I2C_Data);
}

/**
  * ��������: ��ʼ��OLED
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_Init(void)
{
	I2C_InitGPIO();

    rt_thread_mdelay(100); //�������ʱ����Ҫ

    OLED_WriteCmd(0xAE); //display off  �ر���ʾ�������ú������ٴ�

    OLED_WriteCmd(0x20); //Set Memory Addressing Mode    Ѱַģʽ��ҳѰַ Ĭ��Ҳ��ҳѰַ
    OLED_WriteCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid

    OLED_WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCmd(0xc8); //Set COM Output Scan Direction
    OLED_WriteCmd(0x00); //---set low column address
    OLED_WriteCmd(0x10); //---set high column address

    OLED_WriteCmd(0x40); //--set start line address

    OLED_WriteCmd(0x81); //--set contrast control register
    OLED_WriteCmd(0xff); //���ȵ��� 0x00~0xff
    OLED_WriteCmd(0xa1); //--set segment re-map 0 to 127
    OLED_WriteCmd(0xa6); //--set normal display
    OLED_WriteCmd(0xa8); //--set multiplex ratio(1 to 64)

    OLED_WriteCmd(0x3F); //

    OLED_WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    OLED_WriteCmd(0xd3); //-set display offset
    OLED_WriteCmd(0x00); //-not offset
    OLED_WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency

    OLED_WriteCmd(0xf0); //--set divide ratio

    OLED_WriteCmd(0xd9); //--set pre-charge period
    OLED_WriteCmd(0x22); //
    OLED_WriteCmd(0xda); //--set com pins hardware configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xdb); //--set vcomh
    OLED_WriteCmd(0x20); //0x20,0.77xVcc
    OLED_WriteCmd(0x8d); //--set DC-DC enable
    OLED_WriteCmd(0x14); //
    OLED_WriteCmd(0xaf); //--turn on oled panel  ������ʾ
}

/**
  * ��������: ���ù��
  * �������: x�����xλ��
  *           y�����yλ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void OLED_SetPos(uint8_t x, uint8_t y) //������ʼ������
{
    OLED_WriteCmd(0xb0+y);
    OLED_WriteCmd(((x&0xf0)>>4)|0x10);
    OLED_WriteCmd((x&0x0f)|0x01);
}

/**
  * ��������: ���������Ļ
  * �������: fill_Data��Ҫ��������
  * �� �� ֵ: ��
  * ˵    ������
  */
static void OLED_Fill(uint8_t fill_Data)//ȫ�����
{
    uint8_t m,n;
    for(m=0;m<8;m++)
    {
        OLED_WriteCmd(0xb0+m);       //page0-page1
        OLED_WriteCmd(0x00);     //low column start address
        OLED_WriteCmd(0x10);     //high column start address
        for(n=0;n<128;n++)
            {
                OLED_WriteDat(fill_Data);
            }
    }
}

/**
  * ��������: ��OLED�������л���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_ON(void)
{
    OLED_WriteCmd(0X8D);  //���õ�ɱ�
    OLED_WriteCmd(0X14);  //������ɱ�
    OLED_WriteCmd(0XAF);  //OLED����
}

/**
  * ��������: ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_OFF(void)
{
    OLED_WriteCmd(0X8D);  //���õ�ɱ�
    OLED_WriteCmd(0X10);  //�رյ�ɱ�
    OLED_WriteCmd(0XAE);  //OLED����
}

/**
  * ��������: ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
  * �������: x,y�� ��ʼ������(x:0~127, y:0~7);
  *           ch[]��Ҫ��ʾ���ַ���;
  *           TextSize���ַ���С(1:6*8 ; 2:8*16)
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize)
{
    uint8_t c = 0,i = 0,j = 0;
    switch(TextSize)
    {
        case 1:
        {
            while(ch[j] != '\0')
            {
                c = ch[j] - 32;
                if(x > 126)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x,y);
                for(i=0;i<6;i++)
                    OLED_WriteDat(F6x8[c][i]);
                x += 6;
                j++;
            }
        }break;
        case 2:
        {
            while(ch[j] != '\0')
            {
                c = ch[j] - 32;
                if(x > 120)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x,y);
                for(i=0;i<8;i++)
                    OLED_WriteDat(F8X16[c*16+i]);
                OLED_SetPos(x,y+1);
                for(i=0;i<8;i++)
                    OLED_WriteDat(F8X16[c*16+i+8]);
                x += 8;
                j++;
            }
        }break;
    }
}

/**
  * ��������: ��ʾcodetab.h�еĺ���,16*16����
  * �������: x,y����ʼ������(x:0~127, y:0~7)
  *           N��������codetab.h�е�����
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char n,unsigned char * words)
{
    unsigned char wm=0;
    unsigned int  adder=32*n;
    OLED_SetPos(x , y);
    for(wm = 0;wm < 16;wm++)
    {
        OLED_WriteDat(words[adder]);
        adder += 1;
    }
    OLED_SetPos(x,y + 1);
    for(wm = 0;wm < 16;wm++)
    {
        OLED_WriteDat(words[adder]);
        adder += 1;
    }
}

/**
  * ��������: ��Ļ�������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_Clear(void)
{
    OLED_Fill(0x00);
    rt_thread_mdelay(10);
}

/**
  * ��������: OLED_DrawBMP����ʾBMPλͼ
  * �������: x0,y0 :��ʼ������(x0:0~127, y0:0~7)
  *           x1,y1 : ���Խ���(������)������(x1:1~128,y1:1~8)
  * �� �� ֵ: ��
  * ˵    ������
  */
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[])
{
	uint32_t j=0;
    uint8_t x,y;

	if(y1%8==0)
		y = y1/8;
	else
        y = y1/8 + 1;
    for(y=y0;y<y1;y++)
    {
        OLED_SetPos(x0,y);
		for(x=x0;x<x1;x++)
        {
            OLED_WriteDat(BMP[j++]);
        }
    }
}
