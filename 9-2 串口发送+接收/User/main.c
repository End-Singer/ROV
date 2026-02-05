#include "stm32f10x.h" 
#include "Delay.h" 
#include "OLED.h"
#include "Serial.h"
char RxData[20];
int main(void)
{
	OLED_Init();
	Serial_Init();
	OLED_ShowString(1, 1, "RxData:");
	while(1)
	{
		if (Serial_GetRxFlag() == 1)
		{
			sprintf(RxData, "%c", Serial_GetRxData());
			OLED_ShowString(1, 8, RxData);
			Serial_SendString(RxData);
		}
	}
}
