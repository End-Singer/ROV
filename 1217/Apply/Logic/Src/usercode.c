#include "usercode.h"
#include "config.h"

extern tagTIM_T tTimer3;
extern tagTIM_T tTimer4;

// 将逻辑电机号对应到具体的硬件通道
#define M1_TIM (&(tTimer3.tTimerHandle))
#define M1_CH  TIM_CHANNEL_3 // PB0

#define M2_TIM (&(tTimer3.tTimerHandle))
#define M2_CH  TIM_CHANNEL_4 // PB1

#define M3_TIM (&(tTimer4.tTimerHandle))
#define M3_CH  TIM_CHANNEL_1 // PB6

#define M4_TIM (&(tTimer4.tTimerHandle))
#define M4_CH  TIM_CHANNEL_3 // PB8

static uint16_t ESC_Speed_Map(int16_t speed)
{
    // ... 映射逻辑：-100~100 -> 1100~1900 ...
    if (speed > 100) speed = 100;
    if (speed < -100) speed = -100;
    return (uint16_t)(1500 + (speed * 4)); // 简化版示意：±100对应±400us
}

void Set_Four_Motors(int16_t m1, int16_t m2, int16_t m3, int16_t m4)
{
    __HAL_TIM_SET_COMPARE(M1_TIM, M1_CH, ESC_Speed_Map(m1));
    __HAL_TIM_SET_COMPARE(M2_TIM, M2_CH, ESC_Speed_Map(m2));
    __HAL_TIM_SET_COMPARE(M3_TIM, M3_CH, ESC_Speed_Map(m3));
    __HAL_TIM_SET_COMPARE(M4_TIM, M4_CH, ESC_Speed_Map(m4));
}

void UserLogic_Code(void)
{
    static uint8_t init_done = 0;
    if(!init_done) {
        printf("ESC Arming...\r\n");
        Set_Four_Motors(0, 0, 0, 0);
        HAL_Delay(3000); // 必须给 3 秒解锁时间
        init_done = 1;
        printf("ESC Ready!\r\n");
    }
    
    // 测试：四个电机低速转动
    Set_Four_Motors(100, 100, 100, 100);
}
