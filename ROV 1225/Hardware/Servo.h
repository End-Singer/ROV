#ifndef __SERVO_H
#define __SERVO_H

void Servo_Init(void);
void Servo_SetAngle(float Angle);
void Servo_Rotate(uint8_t direction);  // direction: 1=正转, 0=反转

#endif
