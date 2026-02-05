#include "usercode.h"		/* usercode头文件 */
#include "drv_hal_conf.h"   /* SGA库头文件配置 */
#include "task_conf.h"      /* task层头文件配置 */
#include "ocd_conf.h"       /* OCD层头文件配置 */
#include "dev_conf.h"		/* Dev层头文件配置 */
#include "algo_conf.h"		/* Algo层头文件配置 */
#include "config.h"			/* I/O配置头文件配置 */

/* 用户逻辑代码 */
void UserLogic_Code(void)
{
	printf("SGA_DEMO\r\n");
	// 在这里写整体逻辑
	// 点亮LED（PB0输出高电平）
	Drv_GPIO_Set(&ledGPIO[0]);
	
	// 或者使用写入函数，效果相同
	// Drv_GPIO_Write(&ledGPIO[0], GPIO_PIN_SET);
	
	// 如果需要闪烁，可以使用以下代码：
	/*
	while(1)
	{
		Drv_GPIO_Toggle(&ledGPIO[0]);  // 状态翻转
		HAL_Delay(500);                // 延时500ms（需要确保HAL_Delay可用）
	}
	*/
}
