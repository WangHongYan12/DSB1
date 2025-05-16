/**
  ******************************************************************************
  * @file    encoder.h
  * @brief   编码器驱动头文件
  ******************************************************************************
  */

#ifndef __ENCODER_H
#define __ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Type definitions ---------------------------------------------------------*/
typedef enum {
    ENCODER_MOTOR_A = 0,  // 电机A编码器
    ENCODER_MOTOR_B,       // 电机B编码器
    ENCODER_MOTOR_C,       // 电机C编码器
    ENCODER_MOTOR_D        // 电机D编码器
} EncoderMotorID;

static int32_t s_position[4];
/* Function prototypes ------------------------------------------------------*/
void Encoder_Init(void);
int16_t GetEncoder_A(void);
int16_t GetEncoder_B(void);
int16_t GetEncoder_C(void);
int16_t GetEncoder_D(void);
int32_t GetEncoder_Position(EncoderMotorID motor_id);
void Encoder_ResetAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __ENCODER_H */