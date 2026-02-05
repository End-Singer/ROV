#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

// USART1 相关
extern char Serial1_RxPacket[100];
extern uint8_t Serial1_RxFlag;

// USART2 相关
extern char Serial2_RxPacket[100];
extern uint8_t Serial2_RxFlag;

// USART1 初始化函数
void Serial1_Init(void);

// USART2 初始化函数
void Serial2_Init(void);

// USART1 发送函数
void Serial1_SendByte(uint8_t Byte);
void Serial1_SendArray(uint8_t *Array, uint16_t Length);
void Serial1_SendString(char *String);
void Serial1_SendNum(uint32_t Number, uint8_t Length);
void Serial1_Printf(char *format, ...);
uint32_t Serial1_Pow(uint32_t X, uint32_t Y);

// USART2 发送函数
void Serial2_SendByte(uint8_t Byte);
void Serial2_SendArray(uint8_t *Array, uint16_t Length);
void Serial2_SendString(char *String);
void Serial2_SendNum(uint32_t Number, uint8_t Length);
void Serial2_Printf(char *format, ...);
uint32_t Serial2_Pow(uint32_t X, uint32_t Y);

// 重定向 printf 到 USART1
int fputc(int ch, FILE *f);

// USART2 中断服务函数
void USART2_IRQHandler(void);

#endif
