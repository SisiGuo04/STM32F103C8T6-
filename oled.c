#include "font.h"
#include "oled.h"

/*引脚配置*/
#define OLED_W_SCL(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define OLED_W_SDA(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)

/*引脚初始化*/
void OLED_I2C_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();  // 启用 GPIOB 时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;  // 初始化 PB6 和 PB7
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;  // 开漏输出
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);  // 设置 SCL 为高电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);  // 设置 SDA 为高电平
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
    OLED_W_SDA(1);
    OLED_W_SCL(1);
    OLED_W_SDA(0);
    OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
    OLED_W_SDA(0);
    OLED_W_SCL(1);
    OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        OLED_W_SDA(Byte & (0x80 >> i));
        OLED_W_SCL(1);
        OLED_W_SCL(0);
    }
    OLED_W_SCL(1);  //额外的一个时钟，不处理应答信号
    OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);        //从机地址
    OLED_I2C_SendByte(0x00);        //写命令
    OLED_I2C_SendByte(Command); 
    OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);        //从机地址
    OLED_I2C_SendByte(0x40);        //写数据
    OLED_I2C_SendByte(Data);
    OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                    //设置Y位置
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));    //设置X位置高4位
    OLED_WriteCommand(0x00 | (X & 0x0F));           //设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for(i = 0; i < 128; i++)
        {
            OLED_WriteData(0x00);
        }
    }
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      
    uint8_t i;
    uint16_t index = (Char - ' ') * 16;  // 计算字符在F8x16数组中的起始位置
    
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);     // 设置光标位置在上半部分
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(F8x16[index + i]);      // 显示上半部分内容
    }
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); // 设置光标位置在下半部分
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(F8x16[index + i + 8]);  // 显示下半部分内容
    }
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)                            
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0)
    {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++)                            
    {
        OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++)                            
    {
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10)
        {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        }
        else
        {
            OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)                            
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
    }
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
    uint32_t i, j;
    
    for (i = 0; i < 1000; i++)          //上电延时
    {
        for (j = 0; j < 1000; j++);
    }
    
    OLED_I2C_Init();            //端口初始化
    
    OLED_WriteCommand(0xAE);    //关闭显示
    
    OLED_WriteCommand(0xD5);    //设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(0x80);
    
    OLED_WriteCommand(0xA8);    //设置多路复用率
    OLED_WriteCommand(0x3F);
    
    OLED_WriteCommand(0xD3);    //设置显示偏移
    OLED_WriteCommand(0x00);
    
    OLED_WriteCommand(0x40);    //设置显示开始行
    
    OLED_WriteCommand(0xA1);    //设置左右方向，0xA1正常 0xA0左右反置
    
    OLED_WriteCommand(0xC8);    //设置上下方向，0xC8正常 0xC0上下反置

    OLED_WriteCommand(0xDA);    //设置COM引脚硬件配置
    OLED_WriteCommand(0x12);
    
    OLED_WriteCommand(0x81);    //设置对比度控制
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9);    //设置预充电周期
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);    //设置VCOMH取消选择级别
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4);    //设置整个显示打开/关闭

    OLED_WriteCommand(0xA6);    //设置正常/倒转显示

    OLED_WriteCommand(0x8D);    //设置充电泵
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF);    //开启显示
        
    OLED_Clear();               //OLED清屏
}

/**
 * @brief 屏幕亮度调节
 * @param intensity 亮度值，范围：0x00~0xFF，RESET=0x7F
 */
void OLED_IntensityControl(uint8_t intensity)
{
    OLED_WriteCommand(0x81);  // 设置对比度控制命令
    OLED_WriteCommand(intensity);  // 写入亮度值
}

/**
  * @brief 屏幕内容水平全屏滚动播放
  * @param direction 滚动方向，LEFT: 0x27，RIGHT: 0x26
  */
void OLED_HorizontalShift(uint8_t direction)
{
    OLED_WriteCommand(0x2E);  // 停止滚动
    OLED_WriteCommand(direction);  // 设置滚动方向
    OLED_WriteCommand(0x00);  // 虚拟字节设置，默认为0x00
    OLED_WriteCommand(0x00);  // 设置开始页地址
    OLED_WriteCommand(0x07);  // 设置滚动速度（帧频）
    OLED_WriteCommand(0x07);  // 设置结束页地址
    OLED_WriteCommand(0x00);  // 虚拟字节设置，默认为0x00
    OLED_WriteCommand(0xFF);  // 虚拟字节设置，默认为0xFF
    OLED_WriteCommand(0x2F);  // 开启滚动
}

/**
  * @brief 屏幕部分内容水平滚动播放
  * @param direction 滚动方向，LEFT: 0x27，RIGHT: 0x26
  * @param start 开始页地址，范围：0x00~0x07
  * @param end 结束页地址，范围：0x01~0x07
  */
void OLED_Some_HorizontalShift(uint8_t direction, uint8_t start, uint8_t end)
{
    OLED_WriteCommand(0x2E);  // 停止滚动
    OLED_WriteCommand(direction);  // 设置滚动方向
    OLED_WriteCommand(0x00);  // 虚拟字节设置，默认为0x00
    OLED_WriteCommand(start);  // 设置开始页地址
    OLED_WriteCommand(0x07);  // 设置滚动速度（帧频）
    OLED_WriteCommand(end);  // 设置结束页地址
    OLED_WriteCommand(0x00);  // 虚拟字节设置，默认为0x00
    OLED_WriteCommand(0xFF);  // 虚拟字节设置，默认为0xFF
    OLED_WriteCommand(0x2F);  // 开启滚动
}

/**
  * @brief 屏幕内容垂直水平全屏滚动播放
  * @param direction 滚动方向，右上滚动: 0x29，左上滚动: 0x2A
  */
void OLED_VerticalAndHorizontalShift(uint8_t direction)
{
    OLED_WriteCommand(0x2E);  // 停止滚动
    OLED_WriteCommand(direction);  // 设置滚动方向
    OLED_WriteCommand(0x01);  // 虚拟字节设置
    OLED_WriteCommand(0x00);  // 设置开始页地址
    OLED_WriteCommand(0x07);  // 设置滚动速度（帧频）
    OLED_WriteCommand(0x07);  // 设置结束页地址
    OLED_WriteCommand(0x01);  // 垂直滚动偏移量
    OLED_WriteCommand(0x00);  // 虚拟字节设置，默认为0x00
    OLED_WriteCommand(0xFF);  // 虚拟字节设置，默认为0xFF
    OLED_WriteCommand(0x2F);  // 开启滚动
}

/**
  * @brief  OLED显示汉字（修正版）
  * @param  Line 行位置，范围：1~4（对应页地址 0~7）
  * @param  Column 列位置，范围：1~16（实际列地址 = (Column-1)*16）
  * @param  Index 汉字在字库中的索引号
  * @retval 无
  */
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t Index) 
{
    uint8_t i;
    uint16_t offset = Index * 32; // 每个汉字占32字节

    // 设置起始位置（修正坐标计算）
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 16);

    // 写入上半部分（16字节）
    for (i = 0; i < 16; i++) {
        OLED_WriteData(F16x16[offset + i]);
    }

    // 移动到下半部分（Y坐标+1）
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 16);

    // 写入下半部分（16字节）
    for (i = 0; i < 16; i++) {
        OLED_WriteData(F16x16[offset + 16 + i]);
    }
}


/**
  * @brief  OLED显示位图
  * @param  x0,y0 起始点坐标
  * @param  x1,y1 结束点坐标
  * @param  BMP 位图数据数组
  * @retval 无
  */
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{
    uint16_t j = 0;
    uint8_t x, y;
    
    for (y = y0; y < y1; y++)
    {
        OLED_SetCursor(y, x0);
        for (x = x0; x < x1; x++)
        {
            OLED_WriteData(BMP[j++]);
        }
    }
}

/**
  * @brief  OLED显示字符串（支持不同大小）
  * @param  Line 起始行位置
  * @param  Column 起始列位置
  * @param  String 要显示的字符串
  * @param  Size 字体大小 (1:6x8, 2:8x16)
  * @retval 无
  */
void OLED_ShowSizeString(uint8_t Line, uint8_t Column, char *String, uint8_t Size)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        if (Size == 1)
        {
            OLED_SetCursor(Line, Column + i * 6);
            for (uint8_t j = 0; j < 6; j++)
            {
                OLED_WriteData(F6x8[String[i] - ' '][j]);
            }
        }
        else
        {
            OLED_ShowChar(Line, Column + i, String[i]);
        }
    }
}

/**
  * @brief  OLED唤醒
  * @param  无
  * @retval 无
  */
void OLED_WakeUp(void)
{
    OLED_WriteCommand(0X8D); // 设置电荷泵
    OLED_WriteCommand(0X14); // 开启电荷泵
    OLED_WriteCommand(0XAF); // OLED唤醒
}

/**
  * @brief  OLED休眠
  * @param  无
  * @retval 无
  */
void OLED_Sleep(void)
{
    OLED_WriteCommand(0X8D); // 设置电荷泵
    OLED_WriteCommand(0X10); // 关闭电荷泵
    OLED_WriteCommand(0XAE); // OLED休眠
}
/**
 * @brief  OLED_ShowCN，显示codetab.h中的汉字,16*16点阵
 * @param  x,y: 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
 *
 * @retval 无
 */
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char n)
{
    unsigned char wm = 0;
    unsigned int adder = 32 * n;
    OLED_SetPos(x, y);
    for (wm = 0; wm < 16; wm++)
    {
        OLED_WriteData(F16x16[adder]);
        adder += 1;
    }
    OLED_SetPos(x, y + 1);
    for (wm = 0; wm < 16; wm++)
    {
        OLED_WriteData(F16x16[adder]);
        adder += 1;
    }
}

void OLED_SetPos(unsigned char x, unsigned char y) // 设置起始点坐标
{
    OLED_WriteCommand(0xb0 + y);
    OLED_WriteCommand(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCommand((x & 0x0f) | 0x01);
}
