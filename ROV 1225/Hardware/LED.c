#include "stm32f10x.h"                  // STM32F10x系列微控制器头文件

/**
  * @brief  LED初始化函数
  * @note   初始化GPIOA的Pin1和Pin2为推挽输出模式，用于控制LED
  * @param  无
  * @retval 无
  */
void LED_Init(void)
{
	// 使能GPIOA的时钟（APB2总线）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 定义GPIO初始化结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	// 配置GPIO为推挽输出模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	// 配置GPIO引脚为Pin1和Pin2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //| GPIO_Pin_2 
	// 配置GPIO输出速度为50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// 根据配置初始化GPIOA
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 初始状态设置Pin1和Pin2为高电平（LED熄灭）
	GPIO_SetBits(GPIOA, GPIO_Pin_1);// | GPIO_Pin_2
}

/**
  * @brief  点亮LED1
  * @note   将GPIOA_Pin1设置为低电平（LED亮）
  * @param  无
  * @retval 无
  */
void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);  // 设置Pin1为低电平，LED亮
}

/**
  * @brief  熄灭LED1
  * @note   将GPIOA_Pin1设置为高电平（LED灭）
  * @param  无
  * @retval 无
  */
void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);    // 设置Pin1为高电平，LED灭
}

/**
  * @brief  翻转LED1状态
  * @note   如果LED1亮则熄灭，如果熄灭则点亮
  * @param  无
  * @retval 无
  */
void LED1_TURN(void)
{
	// 读取当前Pin1的输出状态
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) == 0){
		// 当前为低电平（LED亮），设置为高电平（LED灭）
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	}
	else {
		// 当前为高电平（LED灭），设置为低电平（LED亮）
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	}
}

/**
  * @brief  点亮LED2
  * @note   将GPIOA_Pin2设置为低电平（LED亮）
  * @param  无
  * @retval 无
  */
void LED2_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);  // 设置Pin2为低电平，LED亮
}

/**
  * @brief  熄灭LED2
  * @note   将GPIOA_Pin2设置为高电平（LED灭）
  * @param  无
  * @retval 无
  */
void LED2_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_2);    // 设置Pin2为高电平，LED灭
}

/**
  * @brief  翻转LED2状态
  * @note   如果LED2亮则熄灭，如果熄灭则点亮
  * @param  无
  * @retval 无
  */
void LED2_TURN(void)
{
	// 读取当前Pin2的输出状态
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2) == 0){
		// 当前为低电平（LED亮），设置为高电平（LED灭）
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
	}
	else {
		// 当前为高电平（LED灭），设置为低电平（LED亮）
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	}
}
