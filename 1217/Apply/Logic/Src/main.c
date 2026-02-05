#include "main.h"
#include "drv_hal_conf.h"   /* 裸机相关定义以及RTOS开关 */ 
#include "task_userinit.h"
#include "usercode.h"       

#if !defined(RTT_ENABLE)
#include "task_sysinit.h"
#endif

int main()
{
    /* 1. 底层系统初始化（时钟、中断等） */
#if !defined(RTT_ENABLE)
    Task_Sys_Init();        
#endif
    
    /* 2. 用户外设初始化 */
    // 这里会调用你修改后的 Task_UserInit，完成 PB 引脚配置和 PWM 启动
    Task_UserInit();        

    /* 3. 运行模式判断 */
#if defined(RTT_ENABLE) || defined(FREERTOS_ENABLE)
    /* 操作系统模式：由内核调度 UserLogic_Code */
    UserLogic_Code(); 
#else
    /* 裸机模式：必须添加 while(1) 循环 */
    // 只有加了循环，你的 Set_Four_Motors 才能持续输出信号控制电机
    while(1)
    {
        UserLogic_Code();   /* 循环执行：控制计算与 PWM 输出 */
        
        // 适当延时，给系统一点喘息时间，防止打印过快挂掉
        HAL_Delay(10);      
    }
#endif
}
