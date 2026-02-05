#include "stm32f10x.h" 
#include "Delay.h" 
#include "OLED.h"
#include "Servo.h"
#include "Key.h"
#include "PWM.h"
#include "Serial.h"
#include <string.h>
// #include "LED.h"
int main(void)
{
    Serial1_Init();      // 串口初始化
	Serial3_Init();
    PWM_Init();         // 推进器初始化
    while (1)
    {
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// USART3测试		
//		if (Serial3_RxFlag == 1)     // 如果接收到数据包
//        {
//			Serial3_SendByte('[');
//            Serial3_SendString("UP");                     // 立即响应
//			Serial3_SendByte(']');
//			Serial3_RxFlag = 0;
//        }

//		
///////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 关键：在主循环中更新所有电机的PWM（非阻塞式）
        PWM_UpdateAll();
        
        if (Serial1_RxFlag == 1)     // 如果接收到数据包
        {
            // 将收到的数据包与预设的指令对比
            if (strcmp(Serial1_RxPacket, "B1 P") == 0)           // 按下"B1", 收到B1 P指令
            {
                Motor1_SetSpeed(1);                            // GPIO6 正转
                Motor2_SetSpeed(1);                            // GPIO7 正转
                Serial1_SendString("[UP]");                     // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B1 R") == 0)     // 松开"B1", 收到B1 R指令
            {
                Motor1_SetSpeed(0);                            // GPIO6 停转
                Motor2_SetSpeed(0);                            // GPIO7 停转
                Serial1_SendString("[UP_STOP]");                // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B2 P") == 0)     // 按下"B2", 收到B2 P指令
            {
                Motor1_SetSpeed(-1);                           // GPIO6 反转
                Motor2_SetSpeed(-1);                           // GPIO7 反转
                Serial1_SendString("[DOWN]");                   // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B2 R") == 0)     // 松开"B2", 收到B2 R指令
            {
                Motor1_SetSpeed(0);                            // GPIO6 停转
                Motor2_SetSpeed(0);                            // GPIO7 停转
                Serial1_SendString("[DOWN_STOP]");              // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B7 P") == 0)     // 按下"B7", 收到B7 P指令
            {
                Motor3_SetSpeed(1);                            // GPIO8 正转
                Motor4_SetSpeed(1);                            // GPIO9 正转
                Serial1_SendString("[GO]");                     // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B7 R") == 0)     // 松开"B7", 收到B7 R指令
            {
                Motor3_SetSpeed(0);                            // GPIO8 停转
                Motor4_SetSpeed(0);                            // GPIO9 停转
                Serial1_SendString("[GO_STOP]");                // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B5 P") == 0)     // 按下"B5", 收到B5 P指令
            {
                Motor3_SetSpeed(-1);                           // GPIO8 反转
                Motor4_SetSpeed(-1);                           // GPIO9 反转
                Serial1_SendString("[BACK]");                   // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B5 R") == 0)     // 松开"B5", 收到B5 R指令
            {
                Motor3_SetSpeed(0);                            // GPIO8 停转
                Motor4_SetSpeed(0);                            // GPIO9 停转
                Serial1_SendString("[BACK_STOP]");              // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B8 P") == 0)     // 按下"B8", 收到B8 P指令
            {
                Motor3_SetSpeed(1);                            // GPIO8 正转
                Motor4_SetSpeed(-1);                           // GPIO9 反转
                Serial1_SendString("[LEFT]");                   // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B8 R") == 0)     // 松开"B8", 收到B8 R指令
            {
                Motor3_SetSpeed(0);                            // GPIO8 停转
                Motor4_SetSpeed(0);                            // GPIO9 停转
                Serial1_SendString("[LEFT_STOP]");              // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B6 P") == 0)     // 按下"B6", 收到B6 P指令
            {
                Motor3_SetSpeed(-1);                           // GPIO8 反转
                Motor4_SetSpeed(1);                            // GPIO9 正转
                Serial1_SendString("[RIGHT]");                  // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B6 R") == 0)     // 松开"B6", 收到B6 R指令
            {
                Motor3_SetSpeed(0);                            // GPIO8 停转
                Motor4_SetSpeed(0);                            // GPIO9 停转
                Serial1_SendString("[RIGHT_STOP]");             // 立即响应
            }
            else if (strcmp(Serial1_RxPacket, "B9 R") == 0)     // 按下"B9", 收到B9 R指令
            {
				Serial1_SendString("[CATCH]");                  // 立即响应
                Servo_Rotate(1);                               // GPIO0 正转
				Serial1_SendString("[DONE]");
			}
            else if (strcmp(Serial1_RxPacket, "B10 R") == 0)    // 按下"B10", 收到B10 R指令
            {
				Serial1_SendString("[THROW]");                  // 立即响应
                Servo_Rotate(0);                               // GPIO0 反转
				Serial1_SendString("[DONE]");
            }
            else                                               // 收到了未知指令
            {
                Serial1_SendString("[ERROR_COMMAND]");          // 串口回传错误指令
            }
            
            Serial1_RxFlag = 0;          // 处理完成后，清零接收标志位
        }
        else if (Serial3_RxFlag == 1)
        {
            // 1. 通过USART1发送从USART3接收到的字符串
            Serial1_SendByte('[');                    // 添加包头 '['
            Serial1_SendString(Serial3_RxPacket);     // 转发内容
            Serial1_SendByte(']'); 					  // 添加包尾 ']'   
            // 2. 清除接收标志
            Serial3_RxFlag = 0;
        }
    }
}
