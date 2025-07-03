#include "font.h"
#include "oled.h"

/*��������*/
#define OLED_W_SCL(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define OLED_W_SDA(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)

/*���ų�ʼ��*/
void OLED_I2C_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();  // ���� GPIOB ʱ��

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7;  // ��ʼ�� PB6 �� PB7
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;  // ��©���
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);  // ���� SCL Ϊ�ߵ�ƽ
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);  // ���� SDA Ϊ�ߵ�ƽ
}

/**
  * @brief  I2C��ʼ
  * @param  ��
  * @retval ��
  */
void OLED_I2C_Start(void)
{
    OLED_W_SDA(1);
    OLED_W_SCL(1);
    OLED_W_SDA(0);
    OLED_W_SCL(0);
}

/**
  * @brief  I2Cֹͣ
  * @param  ��
  * @retval ��
  */
void OLED_I2C_Stop(void)
{
    OLED_W_SDA(0);
    OLED_W_SCL(1);
    OLED_W_SDA(1);
}

/**
  * @brief  I2C����һ���ֽ�
  * @param  Byte Ҫ���͵�һ���ֽ�
  * @retval ��
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
    OLED_W_SCL(1);  //�����һ��ʱ�ӣ�������Ӧ���ź�
    OLED_W_SCL(0);
}

/**
  * @brief  OLEDд����
  * @param  Command Ҫд�������
  * @retval ��
  */
void OLED_WriteCommand(uint8_t Command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);        //�ӻ���ַ
    OLED_I2C_SendByte(0x00);        //д����
    OLED_I2C_SendByte(Command); 
    OLED_I2C_Stop();
}

/**
  * @brief  OLEDд����
  * @param  Data Ҫд�������
  * @retval ��
  */
void OLED_WriteData(uint8_t Data)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);        //�ӻ���ַ
    OLED_I2C_SendByte(0x40);        //д����
    OLED_I2C_SendByte(Data);
    OLED_I2C_Stop();
}

/**
  * @brief  OLED���ù��λ��
  * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
  * @retval ��
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                    //����Yλ��
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));    //����Xλ�ø�4λ
    OLED_WriteCommand(0x00 | (X & 0x0F));           //����Xλ�õ�4λ
}

/**
  * @brief  OLED����
  * @param  ��
  * @retval ��
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
  * @brief  OLED��ʾһ���ַ�
  * @param  Line ��λ�ã���Χ��1~4
  * @param  Column ��λ�ã���Χ��1~16
  * @param  Char Ҫ��ʾ��һ���ַ�����Χ��ASCII�ɼ��ַ�
  * @retval ��
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      
    uint8_t i;
    uint16_t index = (Char - ' ') * 16;  // �����ַ���F8x16�����е���ʼλ��
    
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);     // ���ù��λ�����ϰ벿��
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(F8x16[index + i]);      // ��ʾ�ϰ벿������
    }
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); // ���ù��λ�����°벿��
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(F8x16[index + i + 8]);  // ��ʾ�°벿������
    }
}

/**
  * @brief  OLED��ʾ�ַ���
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  String Ҫ��ʾ���ַ�������Χ��ASCII�ɼ��ַ�
  * @retval ��
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
  * @brief  OLED�η�����
  * @retval ����ֵ����X��Y�η�
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
  * @brief  OLED��ʾ���֣�ʮ���ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~4294967295
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @retval ��
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
  * @brief  OLED��ʾ���֣�ʮ���ƣ�����������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��-2147483648~2147483647
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @retval ��
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
  * @brief  OLED��ʾ���֣�ʮ�����ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~0xFFFFFFFF
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~8
  * @retval ��
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
  * @brief  OLED��ʾ���֣������ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~1111 1111 1111 1111
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~16
  * @retval ��
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
  * @brief  OLED��ʼ��
  * @param  ��
  * @retval ��
  */
void OLED_Init(void)
{
    uint32_t i, j;
    
    for (i = 0; i < 1000; i++)          //�ϵ���ʱ
    {
        for (j = 0; j < 1000; j++);
    }
    
    OLED_I2C_Init();            //�˿ڳ�ʼ��
    
    OLED_WriteCommand(0xAE);    //�ر���ʾ
    
    OLED_WriteCommand(0xD5);    //������ʾʱ�ӷ�Ƶ��/����Ƶ��
    OLED_WriteCommand(0x80);
    
    OLED_WriteCommand(0xA8);    //���ö�·������
    OLED_WriteCommand(0x3F);
    
    OLED_WriteCommand(0xD3);    //������ʾƫ��
    OLED_WriteCommand(0x00);
    
    OLED_WriteCommand(0x40);    //������ʾ��ʼ��
    
    OLED_WriteCommand(0xA1);    //�������ҷ���0xA1���� 0xA0���ҷ���
    
    OLED_WriteCommand(0xC8);    //�������·���0xC8���� 0xC0���·���

    OLED_WriteCommand(0xDA);    //����COM����Ӳ������
    OLED_WriteCommand(0x12);
    
    OLED_WriteCommand(0x81);    //���öԱȶȿ���
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9);    //����Ԥ�������
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);    //����VCOMHȡ��ѡ�񼶱�
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4);    //����������ʾ��/�ر�

    OLED_WriteCommand(0xA6);    //��������/��ת��ʾ

    OLED_WriteCommand(0x8D);    //���ó���
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF);    //������ʾ
        
    OLED_Clear();               //OLED����
}

/**
 * @brief ��Ļ���ȵ���
 * @param intensity ����ֵ����Χ��0x00~0xFF��RESET=0x7F
 */
void OLED_IntensityControl(uint8_t intensity)
{
    OLED_WriteCommand(0x81);  // ���öԱȶȿ�������
    OLED_WriteCommand(intensity);  // д������ֵ
}

/**
  * @brief ��Ļ����ˮƽȫ����������
  * @param direction ��������LEFT: 0x27��RIGHT: 0x26
  */
void OLED_HorizontalShift(uint8_t direction)
{
    OLED_WriteCommand(0x2E);  // ֹͣ����
    OLED_WriteCommand(direction);  // ���ù�������
    OLED_WriteCommand(0x00);  // �����ֽ����ã�Ĭ��Ϊ0x00
    OLED_WriteCommand(0x00);  // ���ÿ�ʼҳ��ַ
    OLED_WriteCommand(0x07);  // ���ù����ٶȣ�֡Ƶ��
    OLED_WriteCommand(0x07);  // ���ý���ҳ��ַ
    OLED_WriteCommand(0x00);  // �����ֽ����ã�Ĭ��Ϊ0x00
    OLED_WriteCommand(0xFF);  // �����ֽ����ã�Ĭ��Ϊ0xFF
    OLED_WriteCommand(0x2F);  // ��������
}

/**
  * @brief ��Ļ��������ˮƽ��������
  * @param direction ��������LEFT: 0x27��RIGHT: 0x26
  * @param start ��ʼҳ��ַ����Χ��0x00~0x07
  * @param end ����ҳ��ַ����Χ��0x01~0x07
  */
void OLED_Some_HorizontalShift(uint8_t direction, uint8_t start, uint8_t end)
{
    OLED_WriteCommand(0x2E);  // ֹͣ����
    OLED_WriteCommand(direction);  // ���ù�������
    OLED_WriteCommand(0x00);  // �����ֽ����ã�Ĭ��Ϊ0x00
    OLED_WriteCommand(start);  // ���ÿ�ʼҳ��ַ
    OLED_WriteCommand(0x07);  // ���ù����ٶȣ�֡Ƶ��
    OLED_WriteCommand(end);  // ���ý���ҳ��ַ
    OLED_WriteCommand(0x00);  // �����ֽ����ã�Ĭ��Ϊ0x00
    OLED_WriteCommand(0xFF);  // �����ֽ����ã�Ĭ��Ϊ0xFF
    OLED_WriteCommand(0x2F);  // ��������
}

/**
  * @brief ��Ļ���ݴ�ֱˮƽȫ����������
  * @param direction �����������Ϲ���: 0x29�����Ϲ���: 0x2A
  */
void OLED_VerticalAndHorizontalShift(uint8_t direction)
{
    OLED_WriteCommand(0x2E);  // ֹͣ����
    OLED_WriteCommand(direction);  // ���ù�������
    OLED_WriteCommand(0x01);  // �����ֽ�����
    OLED_WriteCommand(0x00);  // ���ÿ�ʼҳ��ַ
    OLED_WriteCommand(0x07);  // ���ù����ٶȣ�֡Ƶ��
    OLED_WriteCommand(0x07);  // ���ý���ҳ��ַ
    OLED_WriteCommand(0x01);  // ��ֱ����ƫ����
    OLED_WriteCommand(0x00);  // �����ֽ����ã�Ĭ��Ϊ0x00
    OLED_WriteCommand(0xFF);  // �����ֽ����ã�Ĭ��Ϊ0xFF
    OLED_WriteCommand(0x2F);  // ��������
}

/**
  * @brief  OLED��ʾ���֣������棩
  * @param  Line ��λ�ã���Χ��1~4����Ӧҳ��ַ 0~7��
  * @param  Column ��λ�ã���Χ��1~16��ʵ���е�ַ = (Column-1)*16��
  * @param  Index �������ֿ��е�������
  * @retval ��
  */
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t Index) 
{
    uint8_t i;
    uint16_t offset = Index * 32; // ÿ������ռ32�ֽ�

    // ������ʼλ�ã�����������㣩
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 16);

    // д���ϰ벿�֣�16�ֽڣ�
    for (i = 0; i < 16; i++) {
        OLED_WriteData(F16x16[offset + i]);
    }

    // �ƶ����°벿�֣�Y����+1��
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 16);

    // д���°벿�֣�16�ֽڣ�
    for (i = 0; i < 16; i++) {
        OLED_WriteData(F16x16[offset + 16 + i]);
    }
}


/**
  * @brief  OLED��ʾλͼ
  * @param  x0,y0 ��ʼ������
  * @param  x1,y1 ����������
  * @param  BMP λͼ��������
  * @retval ��
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
  * @brief  OLED��ʾ�ַ�����֧�ֲ�ͬ��С��
  * @param  Line ��ʼ��λ��
  * @param  Column ��ʼ��λ��
  * @param  String Ҫ��ʾ���ַ���
  * @param  Size �����С (1:6x8, 2:8x16)
  * @retval ��
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
  * @brief  OLED����
  * @param  ��
  * @retval ��
  */
void OLED_WakeUp(void)
{
    OLED_WriteCommand(0X8D); // ���õ�ɱ�
    OLED_WriteCommand(0X14); // ������ɱ�
    OLED_WriteCommand(0XAF); // OLED����
}

/**
  * @brief  OLED����
  * @param  ��
  * @retval ��
  */
void OLED_Sleep(void)
{
    OLED_WriteCommand(0X8D); // ���õ�ɱ�
    OLED_WriteCommand(0X10); // �رյ�ɱ�
    OLED_WriteCommand(0XAE); // OLED����
}
/**
 * @brief  OLED_ShowCN����ʾcodetab.h�еĺ���,16*16����
 * @param  x,y: ��ʼ������(x:0~127, y:0~7); N:������codetab.h�е�����
 *
 * @retval ��
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

void OLED_SetPos(unsigned char x, unsigned char y) // ������ʼ������
{
    OLED_WriteCommand(0xb0 + y);
    OLED_WriteCommand(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCommand((x & 0x0f) | 0x01);
}
