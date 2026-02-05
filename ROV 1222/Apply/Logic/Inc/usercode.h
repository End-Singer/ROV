#ifndef	__USERCODE_H_
#define	__USERCODE_H_

#include <stdbool.h>
#include <stdint.h>  /* 添加标准整数类型定义 */

void UserLogic_Code(void);

/* 水下机器人控制系统函数声明 */
void ROV_Init(void);
void ROV_MotorControl(int8_t motor1, int8_t motor2, int8_t motor3, int8_t motor4);
void ROV_StopAll(void);
void ROV_ProcessUARTCommand(void);
void ROV_RelayData(void);

/* 动作控制函数 */
void ROV_MoveUp(uint8_t power);
void ROV_MoveDown(uint8_t power);
void ROV_MoveForward(uint8_t power);
void ROV_MoveBackward(uint8_t power);
void ROV_TurnLeft(uint8_t power);
void ROV_TurnRight(uint8_t power);

#endif
