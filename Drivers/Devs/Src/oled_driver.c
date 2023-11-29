#include "oled_driver.h"

static void oled_writeCmd(uint8_t I2C_Command);
static void oled_writeDat(uint8_t I2C_Data);
static void oled_setPos(uint8_t x, uint8_t y);
static void oled_fill(uint8_t fill_Data);


/**
  * 函数功能: 向OLED写入命令
  * 输入参数: I2C_Command：命令代码
  * 返 回 值: 无
  * 说    明：无
  */
static void oled_writeCmd(uint8_t I2C_Command)//写命令  
{
    I2C_Write(OLED_DEV_ADDR, 0x00, I2C_Command);
}

/**
  * 函数功能: 向OLED写入数据
  * 输入参数: I2C_Data：数据
  * 返 回 值: 无
  * 说    明：无
  */
static void oled_writeDat(uint8_t I2C_Data)//写数据
{
    I2C_Write(OLED_DEV_ADDR, 0x40, I2C_Data);
}

/**
  * 函数功能: 初始化OLED
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void oled_init(void)
{
	I2C_InitGPIO();

    rt_thread_mdelay(100); //这里的延时很重要

    oled_writeCmd(0xAE); //display off  关闭显示，先设置好其他再打开

    oled_writeCmd(0x20); //Set Memory Addressing Mode    寻址模式，页寻址 默认也是页寻址
    oled_writeCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid

    oled_writeCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    oled_writeCmd(0xc8); //Set COM Output Scan Direction
    oled_writeCmd(0x00); //---set low column address
    oled_writeCmd(0x10); //---set high column address

    oled_writeCmd(0x40); //--set start line address

    oled_writeCmd(0x81); //--set contrast control register
    oled_writeCmd(0xff); //亮度调节 0x00~0xff
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
    oled_writeCmd(0xaf); //--turn on oled panel  开启显示
}

/**
  * 函数功能: 设置光标
  * 输入参数: x：光标x位置
  *           y：光标y位置
  * 返 回 值: 无
  * 说    明：无
  */
static void oled_setPos(uint8_t x, uint8_t y) //设置起始点坐标
{
    oled_writeCmd(0xb0+y);
    oled_writeCmd(((x&0xf0)>>4)|0x10);
    oled_writeCmd((x&0x0f)|0x01);
}

/**
  * 函数功能: 填充整个屏幕
  * 输入参数: fill_Data：要填充的数据
  * 返 回 值: 无
  * 说    明：无
  */
static void oled_fill(uint8_t fill_Data)//全屏填充
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
  * 函数功能: 将OLED从休眠中唤醒
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_ON(void)
{
    oled_writeCmd(0X8D);  //设置电荷泵
    oled_writeCmd(0X14);  //开启电荷泵
    oled_writeCmd(0XAF);  //OLED唤醒
}

/**
  * 函数功能: 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void oled_off(void)
{
    oled_writeCmd(0X8D);  //设置电荷泵
    oled_writeCmd(0X10);  //关闭电荷泵
    oled_writeCmd(0XAE);  //OLED休眠
}

/**
  * 函数功能: 显示codetab.h中的ASCII字符,有6*8和8*16可选择
  * 输入参数: x,y： 起始点坐标(x:0~127, y:0~7);
  *           ch[]：要显示的字符串;
  *           TextSize：字符大小(1:6*8 ; 2:8*16)
  * 返 回 值: 无
  * 说    明：无
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
  * 函数功能: 显示codetab.h中的汉字,16*16点阵
  * 输入参数: x,y：起始点坐标(x:0~127, y:0~7)
  *           N：汉字在codetab.h中的索引
  * 返 回 值: 无
  * 说    明：无
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
  * 函数功能: 屏幕画面清空
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void oled_clear(void)
{
    oled_fill(0x00);
    rt_thread_mdelay(10);
}

/**
  * 函数功能: oled_drawBMP，显示BMP位图
  * 输入参数: x0,y0 :起始点坐标(x0:0~127, y0:0~7)
  *           x1,y1 : 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
  * 返 回 值: 无
  * 说    明：无
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
