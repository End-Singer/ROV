#include "stm32f10x.h" 
#include "Delay.h" 
#include "OLED.h"
#include "Servo.h"
#include "Key.h"

uint8_t Key_Num;
float Angle;


int main(void)
{

	OLED_Init();
	Servo_Init();
	Key_Init();
	OLED_ShowString(1, 1, "Angle:");
	
	while(1)
	{
		Delay_ms(5000);
		TIM_SetCompare1(TIM4, 1500);
		Delay_ms(1000);
		TIM_SetCompare1(TIM4, 1400);
		Delay_ms(5000);
		TIM_SetCompare1(TIM4, 1500);
		Delay_ms(1000);
		TIM_SetCompare1(TIM4, 1600);
		Delay_ms(5000);
			
	}



}
