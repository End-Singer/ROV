#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "drv_hal_conf.h"
#include "ocd_conf.h"
#include "algo_conf.h"
#include "dev_conf.h"

/* 用户句柄声明包含区 */

extern tagTIM_T tTimer2;

extern tagGPIO_T demoGPIO[];

extern tagUART_T demoUart;

extern tagGPIO_T PWM_GPIO[];

extern tagPWM_T MT_PWM[];

/* 水下机器人控制系统UART句柄 */
extern tagUART_T rovUart1;  /* USART1: 接收树莓派数据 (PA9: TX, PA10: RX) */
extern tagUART_T rovUart2;  /* USART2: 发送数据到电脑 (PA2: TX, PA3: RX) */

#endif
