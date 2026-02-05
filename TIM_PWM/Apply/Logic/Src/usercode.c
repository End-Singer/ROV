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

	// Drv_PWM_DutyFactorSet(&MT_PWM[0], 10.0f);
	// Drv_Delay_Ms(100);
	Drv_PWM_DutyFactorSet(&MT_PWM[0], 00.0);
	Drv_PWM_DutyFactorSet(&MT_PWM[1], 00.0);
	Drv_PWM_DutyFactorSet(&MT_PWM[2], 00.0);
	Drv_PWM_DutyFactorSet(&MT_PWM[3], 00.0);
	// Drv_Delay_Ms(100);
	// Drv_PWM_DutyFactorSet(&MT_PWM[0], 0.0f);
}
