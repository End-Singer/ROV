#ifndef __PWM_H
#define __PWM_H
void PWM_Init(void);
void PWM_SetServo(uint16_t Compare);
void PWM_SetCompare1(uint16_t Compare);
void PWM_SetCompare2(uint16_t Compare);  // 添加这个声明
void PWM_SetCompare3(uint16_t Compare);  // 添加这个声明
void PWM_SetCompare4(uint16_t Compare);  // 添加这个声明

// 新增推进器控制函数声明
void Motor1_SetSpeed(int8_t direction); // direction: 1=正转, -1=反转, 0=停止
void Motor2_SetSpeed(int8_t direction);
void Motor3_SetSpeed(int8_t direction);
void Motor4_SetSpeed(int8_t direction);
#endif
