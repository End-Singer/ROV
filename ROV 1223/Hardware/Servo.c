#include "stm32f10x.h"                  // Device header
#include "PWM.h"
void Servo_Init(void)
{
	PWM_Init();
	
}
// 0   500
// 180 2500 º∆À„∑≈Àı
void Servo_SetAngle(float Angle)
{
	PWM_SetServo(Angle / 180 * 2000 + 500);

}
