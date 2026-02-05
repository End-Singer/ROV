#include "stm32f10x.h" 
#include "Delay.h" 
#include "OLED.h"
#include "Servo.h"
#include "Key.h"
#include "PWM.h"
#include "Serial.h"
#include <string.h>

int main(void)
{
	Serial_Init();		//串口初始化
	PWM_Init();			//推进器初始化
	
	while (1)
	{
		if (Serial_RxFlag == 1)		//如果接收到数据包
		{
			
			/*将收到的数据包与预设的指令对比，以此决定将要执行的操作*/
			if (strcmp(Serial_RxPacket, "B1 P") == 0)			//按下"A", 收到B1指令
			{
				Motor1_SetSpeed(1);							// GPIO6 正转
				Motor2_SetSpeed(1);							// GPIO7 正转
				Serial_SendString("[ROV_UP_OK]");		//串口回传一个字符串ROV_UP_OK

			}
			else if (strcmp(Serial_RxPacket, "B1 R") == 0)			//按下"A", 收到B1指令
			{
				Motor1_SetSpeed(0);							// GPIO6 正转
				Motor2_SetSpeed(0);							// GPIO7 正转
				Serial_SendString("[ROV_OFF_OK]");		//串口回传一个字符串ROV_UP_OK

			}
			else						//上述所有条件均不满足，即收到了未知指令
			{
				Serial_SendString("[ERROR_COMMAND]");		//串口回传一个字符串ERROR_COMMAND
			}
			
			Serial_RxFlag = 0;			//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
		}
	}

}
