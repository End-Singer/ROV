#include "task_conf.h"
#include "config.h"
#include "drv_hal_gpio.h" // 确保包含这个头文件

extern tagTIM_T tTimer3;
extern tagTIM_T tTimer4;
extern tagGPIO_T motorGPIOGroup[]; // 引用上面定义的数组

void Task_UserInit(void)
{
    /* 1. 初始化引脚 (开启 GPIOB 时钟并设为复用推挽输出) */
    // 这行代码会遍历 motorGPIOGroup 里的 4 个引脚
    Drv_GPIO_Init(motorGPIOGroup, 4); 

    /* 2. 启动 PWM 通道 */
    HAL_TIM_PWM_Start(&(tTimer3.tTimerHandle), TIM_CHANNEL_3); // PB0
    HAL_TIM_PWM_Start(&(tTimer3.tTimerHandle), TIM_CHANNEL_4); // PB1
    HAL_TIM_PWM_Start(&(tTimer4.tTimerHandle), TIM_CHANNEL_1); // PB6
    HAL_TIM_PWM_Start(&(tTimer4.tTimerHandle), TIM_CHANNEL_3); // PB8

    /* 3. 设置初始安全状态 (1500us 停止位) */
    __HAL_TIM_SET_COMPARE(&(tTimer3.tTimerHandle), TIM_CHANNEL_3, 1500);
    __HAL_TIM_SET_COMPARE(&(tTimer3.tTimerHandle), TIM_CHANNEL_4, 1500);
    __HAL_TIM_SET_COMPARE(&(tTimer4.tTimerHandle), TIM_CHANNEL_1, 1500);
    __HAL_TIM_SET_COMPARE(&(tTimer4.tTimerHandle), TIM_CHANNEL_3, 1500);
    
    printf("Motor System Ready: PB0, PB1, PB6, PB8 Initialized.\r\n");
}
