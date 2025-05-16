/**
  ******************************************************************************
  * @file    motor.h
  * @brief   电机驱动头文件
  ******************************************************************************
  */

#ifndef __MOTOR_H
#define __MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Type definitions ----------------------------------------------------------*/
typedef enum {
    MOTOR_CHANNEL_A = 0,  // 电机A
    MOTOR_CHANNEL_B,      // 电机B
    MOTOR_CHANNEL_C,      // 电机C
    MOTOR_CHANNEL_D       // 电机D
} MotorChannel;

/* Function prototypes ------------------------------------------------------*/
void Motor_Init(void);
void Motor_OutPut(int speedA, int speedB, int speedC, int speedD);

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H */