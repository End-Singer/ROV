#include "usercode.h"		/* usercode头文件 */
#include "drv_hal_conf.h"   /* SGA库头文件配置 */
#include "task_conf.h"      /* task层头文件配置 */
#include "ocd_conf.h"       /* OCD层头文件配置 */
#include "dev_conf.h"		/* Dev层头文件配置 */
#include "algo_conf.h"		/* Algo层头文件配置 */
#include "config.h"			/* I/O配置头文件配置 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 全局变量定义 */
static uint8_t uart2_rx_buffer[128];  /* USART2接收缓冲区 */
static uint8_t uart1_rx_buffer[256];  /* USART1接收缓冲区 */
static bool command_received = false; /* 命令接收标志 */
static char command_buffer[64];       /* 命令解析缓冲区 */

/* 水下机器人控制系统初始化 */
void ROV_Init(void)
{
    /* 初始化PWM电机为停止状态 */
    ROV_StopAll();
    
    /* 初始化UART接收缓冲区 */
    memset(uart2_rx_buffer, 0, sizeof(uart2_rx_buffer));
    memset(uart1_rx_buffer, 0, sizeof(uart1_rx_buffer));
    memset(command_buffer, 0, sizeof(command_buffer));
    
    /* 初始化UART DMA模式 */
    Drv_Uart_DMAInit(&rovUart2);
    Drv_Uart_DMAInit(&rovUart1);
    
    printf("ROV Control System Initialized\r\n");
    printf("PWM Frequency: %u Hz\r\n", MT_PWM[0].ulFreq);
    printf("UART Baud Rate: 115200\r\n");
    printf("Waiting for control commands...\r\n");
}

/* 电机控制函数
 * 参数范围: -100 ~ +100 (负值表示反转)
 * motor1: PB6 (垂直推进器1)
 * motor2: PB7 (垂直推进器2) 
 * motor3: PB8 (水平推进器1)
 * motor4: PB9 (水平推进器2)
 */
void ROV_MotorControl(int8_t motor1, int8_t motor2, int8_t motor3, int8_t motor4)
{
    /* 限制占空比在-100到+100之间 */
    if (motor1 < -100) motor1 = -100;
    if (motor1 > 100) motor1 = 100;
    if (motor2 < -100) motor2 = -100;
    if (motor2 > 100) motor2 = 100;
    if (motor3 < -100) motor3 = -100;
    if (motor3 > 100) motor3 = 100;
    if (motor4 < -100) motor4 = -100;
    if (motor4 > 100) motor4 = 100;
    
    /* 设置PWM占空比 */
    Drv_PWM_DutyFactorSet(&MT_PWM[0], (float)motor1);
    Drv_PWM_DutyFactorSet(&MT_PWM[1], (float)motor2);
    Drv_PWM_DutyFactorSet(&MT_PWM[2], (float)motor3);
    Drv_PWM_DutyFactorSet(&MT_PWM[3], (float)motor4);
}

/* 停止所有电机 */
void ROV_StopAll(void)
{
    ROV_MotorControl(0, 0, 0, 0);
}

/* 上浮控制 */
void ROV_MoveUp(uint8_t power)
{
    if (power > 100) power = 100;
    ROV_MotorControl(power, power, 0, 0);
//    printf("上浮: %d%% 功率\r\n", power);
}

/* 下潜控制 */
void ROV_MoveDown(uint8_t power)
{
    if (power > 100) power = 100;
    ROV_MotorControl(-power, -power, 0, 0);
//    printf("下潜: %d%% 功率\r\n", power);
}

/* 前进控制 */
void ROV_MoveForward(uint8_t power)
{
    if (power > 100) power = 100;
    ROV_MotorControl(0, 0, power, power);
//    printf("前进: %d%% 功率\r\n", power);
}

/* 后退控制 */
void ROV_MoveBackward(uint8_t power)
{
    if (power > 100) power = 100;
    ROV_MotorControl(0, 0, -power, -power);
//    printf("后退: %d%% 功率\r\n", power);
}

/* 左转控制 */
void ROV_TurnLeft(uint8_t power)
{
    if (power > 100) power = 100;
    ROV_MotorControl(0, 0, power, -power);
//    printf("左转: %d%% 功率\r\n", power);
}

/* 右转控制 */
void ROV_TurnRight(uint8_t power)
{
    if (power > 100) power = 100;
    ROV_MotorControl(0, 0, -power, power);
//    printf("右转: %d%% 功率\r\n", power);
}

/* 解析UART命令
 * 命令格式: jsb <power> <action>
 * 示例: jsb 50 up, jsb 30 forward, jsb 0 stop
 */
static void ROV_ParseCommand(const char* cmd)
{
    char action[16];
    uint8_t power = 0;
    
    /* 解析命令 */
    if (sscanf(cmd, "jsb %hhu %15s", &power, action) == 2)
    {
 //       printf("收到命令: 功率=%d%%, 动作=%s\r\n", power, action);
        
        /* 根据动作执行相应操作 */
        if (strcmp(action, "up") == 0)
        {
            ROV_MoveUp(power);
        }
        else if (strcmp(action, "down") == 0)
        {
            ROV_MoveDown(power);
        }
        else if (strcmp(action, "forward") == 0)
        {
            ROV_MoveForward(power);
        }
        else if (strcmp(action, "backward") == 0)
        {
            ROV_MoveBackward(power);
        }
        else if (strcmp(action, "left") == 0)
        {
            ROV_TurnLeft(power);
        }
        else if (strcmp(action, "right") == 0)
        {
            ROV_TurnRight(power);
        }
        else if (strcmp(action, "stop") == 0)
        {
            ROV_StopAll();
 //           printf("停止所有电机\r\n");
        }
        else
        {
 //           printf("未知动作: %s\r\n", action);
        }
    }
    else
    {
//        printf("命令格式错误: %s\r\n", cmd);
//        printf("正确格式: jsb <power> <action>\r\n");
//        printf("示例: jsb 50 up, jsb 30 forward, jsb 0 stop\r\n");
    }
}

/* 处理UART命令 */
void ROV_ProcessUARTCommand(void)
{
    static uint16_t rx_index = 0;
    uint16_t rx_len;
    
    /* 检查USART2 DMA接收完成标志 */
    if (rovUart2.tRxInfo.ucDMARxCplt == 1)
    {
        /* 获取接收数据长度 */
        rx_len = rovUart2.tRxInfo.usDMARxLength;
        
        if (rx_len > 0)
        {
            /* 从DMA缓存读取数据到本地缓冲区 */
            memcpy(uart2_rx_buffer, rovUart2.tRxInfo.ucpDMARxCache, rx_len);
            
            /* 处理接收到的数据 */
            for (uint16_t i = 0; i < rx_len; i++)
            {
                char c = uart2_rx_buffer[i];
                
                if (c == '\r' || c == '\n')
                {
                    /* 命令结束 */
                    if (rx_index > 0)
                    {
                        command_buffer[rx_index] = '\0';
                        ROV_ParseCommand(command_buffer);
                        rx_index = 0;
                    }
                }
                else if (rx_index < sizeof(command_buffer) - 1)
                {
                    /* 存储命令字符 */
                    command_buffer[rx_index++] = c;
                }
            }
            
            /* 清除接收完成标志 */
            rovUart2.tRxInfo.ucDMARxCplt = 0;
        }
    }
}

/* 串口数据中继：USART1 -> USART2 */
void ROV_RelayData(void)
{
    static uint8_t relay_buffer[256];
    uint16_t rx_len;
    
    /* 检查USART1 DMA接收完成标志 */
    if (rovUart1.tRxInfo.ucDMARxCplt == 1)
    {
        /* 获取接收数据长度 */
        rx_len = rovUart1.tRxInfo.usDMARxLength;
        
        if (rx_len > 0)
        {
            /* 从DMA缓存读取数据到中继缓冲区 */
            memcpy(relay_buffer, rovUart1.tRxInfo.ucpDMARxCache, rx_len);
            
            /* 通过USART2发送数据到电脑 */
            Drv_Uart_Transmit_DMA(&rovUart2, relay_buffer, rx_len);
            
            /* 清除接收完成标志 */
            rovUart1.tRxInfo.ucDMARxCplt = 0;
            
            /* 可选：在本地也显示接收到的数据 */
//            printf("[中继] 转发 %d 字节数据\r\n", rx_len);
        }
    }
}

/* 用户主逻辑代码 */
void UserLogic_Code(void)
{
    static bool initialized = false;
    
    /* 系统初始化 */
    if (!initialized)
    {
        ROV_Init();
        initialized = true;
    }
    
    /* 处理UART命令 */
    ROV_ProcessUARTCommand();
    
    /* 执行串口数据中继 */
    ROV_RelayData();
    
    /* 其他用户逻辑可以在这里添加 */
}
