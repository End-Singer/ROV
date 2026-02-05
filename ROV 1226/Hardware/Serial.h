#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

// USART1 相关
extern char Serial1_RxPacket[100];
extern uint8_t Serial1_RxFlag;

// USART3 相关
extern char Serial3_RxPacket[100];
extern uint8_t Serial3_RxFlag;

// USART1 初始化函数
void Serial1_Init(void);

// USART3 初始化函数
void Serial3_Init(void);

// USART1 发送函数
void Serial1_SendByte(uint8_t Byte);
void Serial1_SendArray(uint8_t *Array, uint16_t Length);
void Serial1_SendString(char *String);
void Serial1_SendNum(uint32_t Number, uint8_t Length);
void Serial1_Printf(char *format, ...);
uint32_t Serial1_Pow(uint32_t X, uint32_t Y);

// USART3 发送函数
void Serial3_SendByte(uint8_t Byte);
void Serial3_SendArray(uint8_t *Array, uint16_t Length);
void Serial3_SendString(char *String);
void Serial3_SendNum(uint32_t Number, uint8_t Length);
void Serial3_Printf(char *format, ...);
uint32_t Serial3_Pow(uint32_t X, uint32_t Y);

// 重定向 printf 到 USART1
int fputc(int ch, FILE *f);

// USART3 中断服务函数
void USART3_IRQHandler(void);

#endif
