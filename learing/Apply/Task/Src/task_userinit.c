#include "task_conf.h"
#include "ocd_conf.h"
#include "config.h"

/* 外设初始化函数 */
void Task_UserInit(void)
{
	// 初始化外设（PWM、串口、电机接口）
	// 初始化 PWM
    Drv_PWM_Init(&MotorPWM, 50); // 50kHz PWM 频率

}
