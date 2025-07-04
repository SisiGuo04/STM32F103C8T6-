#ifndef __DHT11_H
#define __DHT11_H 
#include "main.h"   

//IO��������
#define DHT11_HIGH     HAL_GPIO_WritePin(GPIOB, DHT11_Pin,	GPIO_PIN_SET)
#define DHT11_LOW      HAL_GPIO_WritePin(GPIOB, DHT11_Pin, GPIO_PIN_RESET)
#define DHT11_DQ_IN    HAL_GPIO_ReadPin(GPIOB, DHT11_Pin)
#define u8 uint8_t

u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);	//����һ���ֽ�
u8 DHT11_Read_Bit(void);	//����һ��λ
u8 DHT11_Check(void);		//����Ƿ����DHT11
void DHT11_Rst(void);		//��λDHT11    
#endif


