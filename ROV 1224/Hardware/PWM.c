#include "stm32f10x.h"                  // Device header
#include "Delay.h" 
void PWM_Init(void)
{
    // 1. 时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM3, ENABLE);  // 添加TIM3时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 2. GPIO配置（正确）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_0;  // 添加PB0
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 3. 定时器TIM4时基配置
    TIM_InternalClockConfig(TIM4);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;  // ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;  // PSC
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
    
    // ========== 添加TIM3时基配置 ==========
    TIM_InternalClockConfig(TIM3);
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;  // ARR，与TIM4相同
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;  // PSC，与TIM4相同
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    // =====================================
    
    // 4. PWM输出配置
    TIM_OCInitTypeDef TIM_OCInit_Structure;
    
    TIM_OCInit_Structure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInit_Structure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInit_Structure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInit_Structure.TIM_Pulse = 1500;  // ← 改成1500，确保电机初始化时停
    
    // 配置TIM4所有四个通道
    TIM_OC1Init(TIM4, &TIM_OCInit_Structure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    TIM_OC2Init(TIM4, &TIM_OCInit_Structure); 
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    TIM_OC3Init(TIM4, &TIM_OCInit_Structure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    TIM_OC4Init(TIM4, &TIM_OCInit_Structure);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    // ========== 添加TIM3通道3配置 ==========
    TIM_OCInit_Structure.TIM_Pulse = 0;  // 确保初始占空比
    TIM_OC3Init(TIM3, &TIM_OCInit_Structure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    // =====================================
    
    // 5. 使能ARR预装载（重要！）
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_ARRPreloadConfig(TIM3, ENABLE);  // 添加TIM3的ARR预装载使能
    
    // 6. 启动定时器
    TIM_Cmd(TIM4, ENABLE);
    TIM_Cmd(TIM3, ENABLE);  // 添加TIM3启动
}


// 舵机控制
void PWM_SetServo(uint16_t Compare) // Compare是CCR的值
{
	TIM_SetCompare3(TIM3, Compare);
}

// PWM.c 新增部分

// 内部函数：平滑改变CCR值
static void Motor_ChangeCCR(uint16_t* currentCCR, uint16_t targetCCR, TIM_TypeDef* TIMx, uint8_t channel)
{
    uint16_t stepDelay = 5; // 固定每步5ms，可以调整这个值改变加速速度
    
    while (*currentCCR != targetCCR) {
        if (*currentCCR < targetCCR) {
            (*currentCCR)++;
        } else {
            (*currentCCR)--;
        }
        
        // 根据通道设置对应的比较寄存器
        switch(channel) {
            case 1: TIM_SetCompare1(TIMx, *currentCCR); break;
            case 2: TIM_SetCompare2(TIMx, *currentCCR); break;
            case 3: TIM_SetCompare3(TIMx, *currentCCR); break;
            case 4: TIM_SetCompare4(TIMx, *currentCCR); break;
        }
        
        Delay_ms(stepDelay);
    }
}

/**
  * @brief  推进器1（PB6/TIM4_CH1）速度控制
  * @param  direction: 1=正转, -1=反转, 0=停止
  * @retval 无
  */
void Motor1_SetSpeed(int8_t direction)
{
    static uint16_t currentCCR = 1500; // 初始值
    uint16_t targetCCR;

    // 设置目标CCR
    if (direction == 1) {
        targetCCR = 2000;      // 正转
    } else if (direction == -1) {
        targetCCR = 1000;      // 反转
    } else {
        targetCCR = 1500;      // 停止
    }
    
    // 如果目标值就是当前值，直接返回
    if (currentCCR == targetCCR) {
        return;
    }
    
    // 如果需要转向（不是简单的停止）
    if (direction != 0 && 
        ((currentCCR > 1500 && targetCCR < 1500) || 
         (currentCCR < 1500 && targetCCR > 1500))) {
        // 先减速到1500
        Motor_ChangeCCR(&currentCCR, 1500, TIM4, 1);
    }
    
    // 加速到目标值
    Motor_ChangeCCR(&currentCCR, targetCCR, TIM4, 1);
}

/**
  * @brief  推进器2（PB7/TIM4_CH2）速度控制
  */
void Motor2_SetSpeed(int8_t direction)
{
    static uint16_t currentCCR = 1500;
    uint16_t targetCCR;

    if (direction == 1) targetCCR = 2000;
    else if (direction == -1) targetCCR = 1000;
    else targetCCR = 1500;
    
    if (currentCCR == targetCCR) return;
    
    if (direction != 0 && 
        ((currentCCR > 1500 && targetCCR < 1500) || 
         (currentCCR < 1500 && targetCCR > 1500))) {
        Motor_ChangeCCR(&currentCCR, 1500, TIM4, 2);
    }
    
    Motor_ChangeCCR(&currentCCR, targetCCR, TIM4, 2);
}

/**
  * @brief  推进器3（PB8/TIM4_CH3）速度控制
  */
void Motor3_SetSpeed(int8_t direction)
{
    static uint16_t currentCCR = 1500;
    uint16_t targetCCR;

    if (direction == 1) targetCCR = 2000;
    else if (direction == -1) targetCCR = 1000;
    else targetCCR = 1500;
    
    if (currentCCR == targetCCR) return;
    
    if (direction != 0 && 
        ((currentCCR > 1500 && targetCCR < 1500) || 
         (currentCCR < 1500 && targetCCR > 1500))) {
        Motor_ChangeCCR(&currentCCR, 1500, TIM4, 3);
    }
    
    Motor_ChangeCCR(&currentCCR, targetCCR, TIM4, 3);
}

/**
  * @brief  推进器4（PB9/TIM4_CH4）速度控制
  */
void Motor4_SetSpeed(int8_t direction)
{
    static uint16_t currentCCR = 1500;
    uint16_t targetCCR;

    if (direction == 1) targetCCR = 2000;
    else if (direction == -1) targetCCR = 1000;
    else targetCCR = 1500;
    
    if (currentCCR == targetCCR) return;
    
    if (direction != 0 && 
        ((currentCCR > 1500 && targetCCR < 1500) || 
         (currentCCR < 1500 && targetCCR > 1500))) {
        Motor_ChangeCCR(&currentCCR, 1500, TIM4, 4);
    }
    
    Motor_ChangeCCR(&currentCCR, targetCCR, TIM4, 4);
}
