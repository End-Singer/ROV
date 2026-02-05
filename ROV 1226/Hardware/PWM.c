#include "stm32f10x.h"                  // Device header
#include "Delay.h" 

// 简化版电机控制结构
typedef struct {
    uint16_t currentCCR;     // 当前CCR值
    uint16_t targetCCR;      // 目标CCR值
    uint16_t finalTargetCCR; // 最终目标CCR（用于方向反转）
    uint8_t state;           // 0=停止, 1=变化中, 2=需要第二阶段
    uint32_t lastUpdateTime; // 上次更新时间
} SimpleMotorCtrl_t;

// 全局电机控制数组
static SimpleMotorCtrl_t motorCtrl[4] = {
    {1500, 1500, 1500, 0, 0},  // 电机1
    {1500, 1500, 1500, 0, 0},  // 电机2
    {1500, 1500, 1500, 0, 0},  // 电机3
    {1500, 1500, 1500, 0, 0}   // 电机4
};

// 初始化函数保持不变
void PWM_Init(void)
{
    // 1. 时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 2. GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 3. 定时器TIM4时基配置
    TIM_InternalClockConfig(TIM4);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
    
    // 4. 定时器TIM3时基配置
    TIM_InternalClockConfig(TIM3);
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    
    // 5. PWM输出配置
    TIM_OCInitTypeDef TIM_OCInit_Structure;
    TIM_OCInit_Structure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInit_Structure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInit_Structure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInit_Structure.TIM_Pulse = 1500;
    
    // TIM4四个通道
    TIM_OC1Init(TIM4, &TIM_OCInit_Structure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2Init(TIM4, &TIM_OCInit_Structure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC3Init(TIM4, &TIM_OCInit_Structure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC4Init(TIM4, &TIM_OCInit_Structure);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    // TIM3通道3
    TIM_OCInit_Structure.TIM_Pulse = 0;
    TIM_OC3Init(TIM3, &TIM_OCInit_Structure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    // 6. 使能ARR预装载
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    
    // 7. 启动定时器
    TIM_Cmd(TIM4, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

// 舵机控制保持不变
void PWM_SetServo(uint16_t Compare)
{
    TIM_SetCompare3(TIM3, Compare);
}

/**
  * @brief  更新单个电机的PWM输出（简化版）
  * @param  index: 电机索引（0-3）
  * @retval 无
  */
static void UpdateMotorPWM(uint8_t index)
{
    SimpleMotorCtrl_t* mc = &motorCtrl[index];
    
    if (mc->state == 0) return; // 不需要更新
    
    uint32_t currentTime = SysTick->VAL;
    uint32_t elapsed = (currentTime > mc->lastUpdateTime) ? 
                      (currentTime - mc->lastUpdateTime) : 
                      (0xFFFFFFFF - mc->lastUpdateTime + currentTime);
    
    // 每1ms更新一次（可调整）
    if (elapsed < 72000) return; // 72MHz/1000 = 72000 ticks/ms
    
    // 执行更新
    if (mc->currentCCR < mc->targetCCR) {
        mc->currentCCR++;
    } else if (mc->currentCCR > mc->targetCCR) {
        mc->currentCCR--;
    }
    
    // 设置PWM输出
    switch(index) {
        case 0: TIM_SetCompare1(TIM4, mc->currentCCR); break;
        case 1: TIM_SetCompare2(TIM4, mc->currentCCR); break;
        case 2: TIM_SetCompare3(TIM4, mc->currentCCR); break;
        case 3: TIM_SetCompare4(TIM4, mc->currentCCR); break;
    }
    
    mc->lastUpdateTime = currentTime;
    
    // 检查是否到达目标
    if (mc->currentCCR == mc->targetCCR) {
        if (mc->state == 2) {
            // 开始第二阶段：从停止到最终目标
            mc->targetCCR = mc->finalTargetCCR;
            mc->state = 1;
        } else {
            mc->state = 0; // 完成
        }
    }
}

/**
  * @brief  更新所有电机的PWM输出（主循环调用）
  * @retval 无
  */
void PWM_UpdateAll(void)
{
    for (uint8_t i = 0; i < 4; i++) {
        UpdateMotorPWM(i);
    }
}

/**
  * @brief  推进器1（PB6/TIM4_CH1）速度控制
  * @param  direction: 1=正转, -1=反转, 0=停止
  * @retval 无
  */
void Motor1_SetSpeed(int8_t direction)
{
    SimpleMotorCtrl_t* mc = &motorCtrl[0];
    uint16_t target;
    
    // 确定目标值
    if (direction == 1) {
        target = 2000;
    } else if (direction == -1) {
        target = 1000;
    } else {
        target = 1500;
    }
    
    // 如果已经在目标值，直接返回
    if (mc->currentCCR == target && mc->state == 0) {
        return;
    }
    
    // 检查是否需要方向反转
    if (direction != 0 && 
        ((mc->currentCCR > 1500 && target < 1500) || 
         (mc->currentCCR < 1500 && target > 1500))) {
        // 需要反转：先到1500
        mc->targetCCR = 1500;
        mc->finalTargetCCR = target; // 记住最终目标
        mc->state = 2; // 标记需要第二阶段
    } else {
        // 直接变化
        mc->targetCCR = target;
        mc->finalTargetCCR = target;
        mc->state = 1;
    }
}

/**
  * @brief  推进器2（PB7/TIM4_CH2）速度控制
  * @param  direction: 1=正转, -1=反转, 0=停止
  * @retval 无
  */
void Motor2_SetSpeed(int8_t direction)
{
    SimpleMotorCtrl_t* mc = &motorCtrl[1];
    uint16_t target;
    
    if (direction == 1) target = 2000;
    else if (direction == -1) target = 1000;
    else target = 1500;
    
    if (mc->currentCCR == target && mc->state == 0) return;
    
    if (direction != 0 && 
        ((mc->currentCCR > 1500 && target < 1500) || 
         (mc->currentCCR < 1500 && target > 1500))) {
        mc->targetCCR = 1500;
        mc->finalTargetCCR = target;
        mc->state = 2;
    } else {
        mc->targetCCR = target;
        mc->finalTargetCCR = target;
        mc->state = 1;
    }
}

/**
  * @brief  推进器3（PB8/TIM4_CH3）速度控制
  * @param  direction: 1=正转, -1=反转, 0=停止
  * @retval 无
  */
void Motor3_SetSpeed(int8_t direction)
{
    SimpleMotorCtrl_t* mc = &motorCtrl[2];
    uint16_t target;
    
    if (direction == 1) target = 2000;
    else if (direction == -1) target = 1000;
    else target = 1500;
    
    if (mc->currentCCR == target && mc->state == 0) return;
    
    if (direction != 0 && 
        ((mc->currentCCR > 1500 && target < 1500) || 
         (mc->currentCCR < 1500 && target > 1500))) {
        mc->targetCCR = 1500;
        mc->finalTargetCCR = target;
        mc->state = 2;
    } else {
        mc->targetCCR = target;
        mc->finalTargetCCR = target;
        mc->state = 1;
    }
}

/**
  * @brief  推进器4（PB9/TIM4_CH4）速度控制
  * @param  direction: 1=正转, -1=反转, 0=停止
  * @retval 无
  */
void Motor4_SetSpeed(int8_t direction)
{
    SimpleMotorCtrl_t* mc = &motorCtrl[3];
    uint16_t target;
    
    if (direction == 1) target = 2000;
    else if (direction == -1) target = 1000;
    else target = 1500;
    
    if (mc->currentCCR == target && mc->state == 0) return;
    
    if (direction != 0 && 
        ((mc->currentCCR > 1500 && target < 1500) || 
         (mc->currentCCR < 1500 && target > 1500))) {
        mc->targetCCR = 1500;
        mc->finalTargetCCR = target;
        mc->state = 2;
    } else {
        mc->targetCCR = target;
        mc->finalTargetCCR = target;
        mc->state = 1;
    }
}
