#include "oled_driver.h"

static void oled_writeCmd(uint8_t I2C_Command);
static void oled_writeDat(uint8_t I2C_Data);
static void oled_setPos(uint8_t x, uint8_t y);
static void oled_fill(uint8_t fill_Data);


/**
  * ��������: ��OLEDд������
  * �������: I2C_Command���������
  * �� �� ֵ: ��
  * ˵    ������
  */
static void oled_writeCmd(uint8_t I2C_Command)//д����  
{
    I2C_Write(OLED_DEV_ADDR, 0x00, I2C_Command);
}

/**
  * ��������: ��OLEDд������
  * �������: I2C_Data������
  * �� �� ֵ: ��
  * ˵    ������
  */
static void oled_writeDat(uint8_t I2C_Data)//д����
{
    I2C_Write(OLED_DEV_ADDR, 0x40, I2C_Data);
}

/**
  * ��������: ��ʼ��OLED
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void oled_init(void)
{
	I2C_InitGPIO();

    rt_thread_mdelay(100); //�������ʱ����Ҫ

    oled_writeCmd(0xAE); //display off  �ر���ʾ�������ú������ٴ�

    oled_writeCmd(0x20); //Set Memory Addressing Mode    Ѱַģʽ��ҳѰַ Ĭ��Ҳ��ҳѰַ
    oled_writeCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid

    oled_writeCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    oled_writeCmd(0xc8); //Set COM Output Scan Direction
    oled_writeCmd(0x00); //---set low column address
    oled_writeCmd(0x10); //---set high column address

    oled_writeCmd(0x40); //--set start line address

    oled_writeCmd(0x81); //--set contrast control register
    oled_writeCmd(0xff); //���ȵ��� 0x00~0xff
    oled_writeCmd(0xa1); //--set segment re-map 0 to 127
    oled_writeCmd(0xa6); //--set normal display
    oled_writeCmd(0xa8); //--set multiplex ratio(1 to 64)

    oled_writeCmd(0x3F); //

    oled_writeCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    oled_writeCmd(0xd3); //-set display offset
    oled_writeCmd(0x00); //-not offset
    oled_writeCmd(0xd5); //--set display clock divide ratio/oscillator frequency

    oled_writeCmd(0xf0); //--set divide ratio

    oled_writeCmd(0xd9); //--set pre-charge period
    oled_writeCmd(0x22); //
    oled_writeCmd(0xda); //--set com pins hardware configuration
    oled_writeCmd(0x12);
    oled_writeCmd(0xdb); //--set vcomh
    oled_writeCmd(0x20); //0x20,0.77xVcc
    oled_writeCmd(0x8d); //--set DC-DC enable
    oled_writeCmd(0x14); //
    oled_writeCmd(0xaf); //--turn on oled panel  ������ʾ
}

/**
  * ��������: ���ù��
  * �������: x�����xλ��
  *           y�����yλ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void oled_setPos(uint8_t x, uint8_t y) //������ʼ������
{
    oled_writeCmd(0xb0+y);
    oled_writeCmd(((x&0xf0)>>4)|0x10);
    oled_writeCmd((x&0x0f)|0x01);
}

/**
  * ��������: ���������Ļ
  * �������: fill_Data��Ҫ��������
  * �� �� ֵ: ��
  * ˵    ������
  */
static void oled_fill(uint8_t fill_Data)//ȫ�����
{
    uint8_t m,n;
    for(m=0;m<8;m++)
    {
        oled_writeCmd(0xb0+m);       //page0-page1
        oled_writeCmd(0x00);     //low column start address
        oled_writeCmd(0x10);     //high column start address
        for(n=0;n<128;n++)
            {
                oled_writeDat(fill_Data);
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
    oled_writeCmd(0X8D);  //���õ�ɱ�
    oled_writeCmd(0X14);  //������ɱ�
    oled_writeCmd(0XAF);  //OLED����
}

/**
  * ��������: ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void oled_off(void)
{
    oled_writeCmd(0X8D);  //���õ�ɱ�
    oled_writeCmd(0X10);  //�رյ�ɱ�
    oled_writeCmd(0XAE);  //OLED����
}

/**
  * ��������: ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
  * �������: x,y�� ��ʼ������(x:0~127, y:0~7);
  *           ch[]��Ҫ��ʾ���ַ���;
  *           TextSize���ַ���С(1:6*8 ; 2:8*16)
  * �� �� ֵ: ��
  * ˵    ������
  */
void oled_showStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize)
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
                oled_setPos(x,y);
                for(i=0;i<6;i++)
                    oled_writeDat(F6x8[c][i]);
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
                oled_setPos(x,y);
                for(i=0;i<8;i++)
                    oled_writeDat(F8X16[c*16+i]);
                oled_setPos(x,y+1);
                for(i=0;i<8;i++)
                    oled_writeDat(F8X16[c*16+i+8]);
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
void oled_showCN(unsigned char x, unsigned char y, unsigned char n,unsigned char * words)
{
    unsigned char wm=0;
    unsigned int  adder=32*n;
    oled_setPos(x , y);
    for(wm = 0;wm < 16;wm++)
    {
        oled_writeDat(words[adder]);
        adder += 1;
    }
    oled_setPos(x,y + 1);
    for(wm = 0;wm < 16;wm++)
    {
        oled_writeDat(words[adder]);
        adder += 1;
    }
}

/**
  * ��������: ��Ļ�������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void oled_clear(void)
{
    oled_fill(0x00);
    rt_thread_mdelay(10);
}

/**
  * ��������: oled_drawBMP����ʾBMPλͼ
  * �������: x0,y0 :��ʼ������(x0:0~127, y0:0~7)
  *           x1,y1 : ���Խ���(������)������(x1:1~128,y1:1~8)
  * �� �� ֵ: ��
  * ˵    ������
  */
void oled_drawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t BMP[])
{
	uint32_t j=0;
    uint8_t x,y;

	if(y1%8==0)
		y = y1/8;
	else
        y = y1/8 + 1;
    for(y=y0;y<y1;y++)
    {
        oled_setPos(x0,y);
		for(x=x0;x<x1;x++)
        {
            oled_writeDat(BMP[j++]);
        }
    }
}
