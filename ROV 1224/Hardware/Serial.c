#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

char Serial_RxPacket[100];
uint8_t Serial_RxFlag;
void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// 中断接收
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_Cmd(USART1, ENABLE);
	
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length) // 发送数组函数
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
		
	}
}

void Serial_SendString(char *String) // 发送字符串
{
	uint16_t i;
	for (i = 0; String[i] != 0; i ++)
	{
		Serial_SendByte(String[i]);
	}
}


uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}
void Serial_SendNum(uint32_t Num, uint16_t Length) // 发送数字
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		// 这行代码的作用是从高位到低位逐位提取数字，转换为ASCII字符后通过串口发送。
		/* Send：1234，i = 1，Length = 4 - 1 - 1 = 2，10^2 = 100,
			1234 / 100 = 12, 12 % 10 = 2, 0的ASCII是48, 2 + 48 发送 2 */
		Serial_SendByte(Num / Serial_Pow(10, Length - i - 1) % 10 + '0'); 
	}
}

// 移植printf函数
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}


// sprintf函数封装
void Serial_printf(char *format, ...)
{
	char string[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(string, format, arg);
	va_end(arg);
	Serial_SendString(string);
}




void USART1_IRQHandler(void)
{
    static uint8_t RxState = 0;
    static uint8_t pRxPacked = 0;
    if (USART_GetFlagStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint8_t RxData = USART_ReceiveData(USART1);
        
        if (RxState == 0)
        {
            if (RxData == '[' && Serial_RxFlag == 0)
            {
                RxState = 1;
                pRxPacked = 0;
            }
        }
        else if (RxState == 1)
        {    
            if (RxData == ']')
            {
                // 修改：直接结束数据包，不需要等待\n
                Serial_RxPacket[pRxPacked] = '\0';
                Serial_RxFlag = 1;
                RxState = 0;  // 重置状态
            }
            else
            {
                if (pRxPacked < 99)  // 防止数组越界
                {
                    Serial_RxPacket[pRxPacked] = RxData;
                    pRxPacked ++;
                }
            }
        }
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
