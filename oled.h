#ifndef __OLED_H
#define __OLED_H
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "gpio.h"
#include "main.h"


void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_IntensityControl(uint8_t intensity);
void OLED_ShowDecimal(uint8_t x, uint8_t y, float num, uint8_t z_len, uint8_t f_len, uint8_t size2, uint8_t Color_Turn);
void OLED_HorizontalShift(uint8_t direction);
void OLED_Some_HorizontalShift(uint8_t direction, uint8_t start, uint8_t end);
void OLED_VerticalAndHorizontalShift(uint8_t direction);
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t Index);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]);
void OLED_ShowSizeString(uint8_t Line, uint8_t Column, char *String, uint8_t Size);
void OLED_WakeUp(void);
void OLED_Sleep(void);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char n);
void OLED_SetPos(unsigned char x, unsigned char y);
#endif

