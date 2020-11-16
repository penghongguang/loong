#ifndef __LCD_H_
#define __LCD_H_
#include "rtthread.h"
#include "ls1c_pin.h"
#include "ls1c_gpio.h"

#define LCD_SCL_PIN 76
#define LCD_SDA_PIN 58
#define LCD_RST_PIN 59
#define LCD_DC_PIN  60
/*sbit LCD_SCL=P1^2;       //SCLK  时钟 D0（SCLK）
sbit LCD_SDA=P1^3;       //SDA   D1（MOSI） 数据
sbit LCD_RST=P1^7;       //_RES  hardware reset   复位 
sbit LCD_DC=P0^0;
*/
void DelayMS(rt_uint32_t msec);
void LCD_DLY_ms(rt_uint32_t ms);
void LCD_WrDat(rt_uint8_t dat);
void LCD_WrCmd(rt_uint8_t cmd);
void LCD_Set_Pos(rt_uint8_t x, rt_uint8_t y);
void LCD_Fill(rt_uint8_t bmp_dat);
void LCD_CLS(void);
void LCD_Init(void);
//void LCD_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[]);
void LCD_showchar(rt_uint8_t x, rt_uint8_t y, rt_uint8_t N);
void LCD_showstr(rt_uint8_t x, rt_uint8_t y,rt_uint8_t ch[]);
void LCD_showint16(rt_uint8_t x, rt_uint8_t y, int ch);
#endif

