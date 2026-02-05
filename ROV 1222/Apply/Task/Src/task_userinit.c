#include "task_conf.h"
#include "ocd_conf.h"
#include "config.h"
#include "usercode.h"

/* 外设初始化函数 */
void Task_UserInit(void)
{
	/* 初始化PWM GPIO和PWM */
	Drv_GPIO_Init(PWM_GPIO, 4);
	Drv_PWM_Init(MT_PWM, 4);
	
	/* 初始化水下机器人控制系统UART */
	Drv_Uart_DMAInit(&rovUart1);
	Drv_Uart_DMAInit(&rovUart2);
	
	/* 初始化水下机器人控制系统 */
	ROV_Init();

}
