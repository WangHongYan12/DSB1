/**
  ******************************************************************************
  * @file    ax_motor.c
  * @brief   四路电机PWM驱动（基于STM32 HAL库）
  * @author  王泓俨
  * @version V1.0
  * @date    2023-04-25
  ******************************************************************************
  * @attention
  * - 使用TIM1的4个通道输出PWM
  * - 需要配合H桥电路控制电机正反转
  * - 输入速度范围：-1000 ~ +1000（对应占空比0%~100%）
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ax_motor.h"
#include "tim.h"
#include "gpio.h"

/* Private defines -----------------------------------------------------------*/
#define MOTOR_PWM_MAX     1000   // PWM最大值（对应100%占空比）

/* Private functions ---------------------------------------------------------*/
static void Set_Single_Motor(MotorChannel channel, int speed);

/* Public functions ----------------------------------------------------------*/

/**
  * @brief  初始化电机PWM输出
  * @note   必须在主循环开始前调用一次
  */
void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
}

/**
  * @brief  设置四个电机的PWM输出
  * @param  speedA 电机A速度（-MOTOR_PWM_MAX ~ +MOTOR_PWM_MAX）
  * @param  speedB 电机B速度（同上）
  * @param  speedC 电机C速度（同上）
  * @param  speedD 电机D速度（同上）
  */
void Motor_OutPut(int speedA, int speedB, int speedC, int speedD)
{
    Set_Single_Motor(MOTOR_CHANNEL_A, speedA);
    Set_Single_Motor(MOTOR_CHANNEL_B, speedB);
    Set_Single_Motor(MOTOR_CHANNEL_C, speedC);
    Set_Single_Motor(MOTOR_CHANNEL_D, speedD);
}

/* Private functions --------------------------------------------------------*/

/**
  * @brief  设置单个电机输出（内部使用）
  * @param  channel 电机通道标识
  * @param  speed   目标速度（带方向）
  */
static void Set_Single_Motor(MotorChannel channel, int speed)
{
    GPIO_PinState pin1_state, pin2_state;
    uint32_t pwm_channel;
    GPIO_TypeDef* in1_port, *in2_port;
    uint16_t in1_pin, in2_pin;

    /* 根据通道选择对应GPIO和TIM通道 */
    switch (channel) {
        case MOTOR_CHANNEL_A:
            in1_port = AIN1_GPIO_Port; in1_pin = AIN1_Pin;
            in2_port = AIN2_GPIO_Port; in2_pin = AIN2_Pin;
            pwm_channel = TIM_CHANNEL_1;
            break;
        case MOTOR_CHANNEL_B:
            in1_port = BIN1_GPIO_Port; in1_pin = BIN1_Pin;
            in2_port = BIN2_GPIO_Port; in2_pin = BIN2_Pin;
            pwm_channel = TIM_CHANNEL_2;
            break;
        case MOTOR_CHANNEL_C:
            in1_port = CIN1_GPIO_Port; in1_pin = CIN1_Pin;
            in2_port = CIN2_GPIO_Port; in2_pin = CIN2_Pin;
            pwm_channel = TIM_CHANNEL_3;
            break;
        case MOTOR_CHANNEL_D:
            in1_port = DIN1_GPIO_Port; in1_pin = DIN1_Pin;
            in2_port = DIN2_GPIO_Port; in2_pin = DIN2_Pin;
            pwm_channel = TIM_CHANNEL_4;
            break;
        default:
            return;
    }

    /* 速度限幅 */
    speed = (speed > MOTOR_PWM_MAX) ? MOTOR_PWM_MAX :
            (speed < -MOTOR_PWM_MAX) ? -MOTOR_PWM_MAX : speed;

    /* 设置方向控制引脚 */
    if (speed >= 0) {
        pin1_state = GPIO_PIN_SET;
        pin2_state = GPIO_PIN_RESET;
    } else {
        pin1_state = GPIO_PIN_RESET;
        pin2_state = GPIO_PIN_SET;
        speed = -speed;  // 转换为正数
    }

    HAL_GPIO_WritePin(in1_port, in1_pin, pin1_state);
    HAL_GPIO_WritePin(in2_port, in2_pin, pin2_state);
            __HAL_TIM_SetCompare(&htim1, pwm_channel, (uint32_t)speed);
}

/************************ (C) COPYRIGHT [公司/作者] *****END OF FILE****/