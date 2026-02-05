#include "task_conf.h"
#include "ocd_conf.h"
#include "config.h"

/* 外设初始化函数 */
void Task_UserInit(void)
{
	// 初始化GPIO
	Drv_GPIO_Init(ledGPIO, 1); 
	
	// 初始化其他外设（PWM、串口、电机接口）...
}
