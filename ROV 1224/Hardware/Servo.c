#include "stm32f10x.h"
#include "PWM.h"
#include "Servo.h"
#include "Delay.h" 

// 全局变量，记录当前角度
static float currentAngle = 0.0f;

void Servo_Init(void)
{
    PWM_Init();
    // 初始化舵机到0度位置
    currentAngle = 0.0f;
    Servo_SetAngle(currentAngle);
}

void Servo_SetAngle(float Angle)
{
    PWM_SetServo(Angle / 180 * 2000 + 500);
}

/**
  * @brief  舵机控制函数（缓慢转动）
  * @param  direction: 1=正转(0→180度), 0=反转(180→0度)
  * @retval 无
  * @note   转速缓慢，全程约15秒
  */
void Servo_Rotate(uint8_t direction)
{
    float targetAngle;
    
    if (direction == 1) {  // 正转：0→180度
        targetAngle = 180.0f;
        
        // 缓慢增加到180度
        while (currentAngle < targetAngle) {
            currentAngle += 0.5f;  // 每次增加0.5度
            if (currentAngle > 180.0f) {
                currentAngle = 180.0f;
            }
            Servo_SetAngle(currentAngle);
            Delay_ms(50);  // 每步延时50ms
        }
    } 
    else {  // 反转：180→0度
        targetAngle = 0.0f;
        
        // 缓慢减少到0度
        while (currentAngle > targetAngle) {
            currentAngle -= 0.5f;  // 每次减少0.5度
            if (currentAngle < 0.0f) {
                currentAngle = 0.0f;
            }
            Servo_SetAngle(currentAngle);
            Delay_ms(50);  // 每步延时50ms
        }
    }
}
