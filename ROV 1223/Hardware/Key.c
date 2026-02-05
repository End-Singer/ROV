#include "stm32f10x.h"                  // STM32F10x系列微控制器头文件
#include "Delay.h"                      // 延时函数头文件

/**
  * @brief  按键初始化函数
  * @note   初始化GPIOB的Pin1和Pin11为上拉输入模式，用于检测按键
  * @param  无
  * @retval 无
  */
void Key_Init(void)
{
	// 使能GPIOB的时钟（APB2总线）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// 定义GPIO初始化结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	// 配置GPIO为上拉输入模式（内部上拉电阻，按键按下时接地）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	// 配置GPIO引脚为Pin1和Pin11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	// 配置GPIO速度（对于输入模式，速度设置影响不大）
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// 根据配置初始化GPIOB
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  获取按键编号
  * @note   检测两个按键的状态，返回被按下的按键编号（支持按键消抖）
  * @param  无
  * @retval 按键编号：0-无按键，1-按键1(PB1)，2-按键2(PB11)
  */
uint8_t Key_GetNum(void)
{
	uint8_t Key_Num = 0;  // 默认返回0，表示无按键按下
	
	// 检测按键1（PB1）是否按下
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		// 延时20ms进行按键消抖，消除机械抖动
		Delay_ms(20);
		
		// 等待按键释放（阻塞式等待）
		// 当按键保持按下时，循环空转；按键松开时退出循环
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0);
		
		// 再次延时20ms，进行松开消抖
		Delay_ms(20);
		
		// 设置返回值为1，表示按键1被按下
		Key_Num = 1;
	}
	
	// 检测按键2（PB11）是否按下
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
		// 延时20ms进行按键消抖
		Delay_ms(20);
		
		// 等待按键释放
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0);
		
		// 再次延时20ms，进行松开消抖
		Delay_ms(20);
		
		// 设置返回值为2，表示按键2被按下
		Key_Num = 2;
	}
	
	// 返回按键编号（0：无按键，1：按键1，2：按键2）
	return Key_Num;
}
