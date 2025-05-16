/**
  ******************************************************************************
  * @file    encoder.c
  * @brief   四路编码器接口驱动（基于STM32 HAL库）
  * @author  why
  * @version V1.0
  * @date    25/4/25
  ******************************************************************************
  * @attention
  * - 依赖STM32 HAL库和TIM外设
  * - 编码器模式需在CubeMX中配置为Encoder Mode
  * - 每个编码器对应独立的TIMx（TIM2/TIM3/TIM4/TIM5）
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "ax_encoder.h"
/* Private variables ---------------------------------------------------------*/
static int32_t s_position[4] = {100,100,100,100};  ///< 各电机累计位置（A/B/C/D对应索引0/1/2/3）

/* Private function prototypes -----------------------------------------------*/
static int16_t Encoder_Read(TIM_HandleTypeDef *htim);

/* Public functions ---------------------------------------------------------*/

/**
  * @brief  初始化所有编码器接口
  * @note   必须在主循环开始前调用一次
  */
void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
}

/**
  * @brief  获取电机A的编码器增量值
  * @retval 本次读取的脉冲增量（带方向）
  */
int16_t GetEncoder_A(void)
{
    int16_t delta = Encoder_Read(&htim2);
    s_position[0] += delta;
    return delta;
}

/**
  * @brief  获取电机B的编码器增量值
  * @retval 本次读取的脉冲增量（带方向）
  */
int16_t GetEncoder_B(void)
{
    int16_t delta = Encoder_Read(&htim3);
    s_position[1] += delta;
    return delta;
}

/**
  * @brief  获取电机C的编码器增量值
  * @retval 本次读取的脉冲增量（带方向）
  */
int16_t GetEncoder_C(void)
{
    int16_t delta = Encoder_Read(&htim4);
    s_position[2] += delta;
    return delta;
}

/**
  * @brief  获取电机D的编码器增量值
  * @retval 本次读取的脉冲增量（带方向）
  */
int16_t GetEncoder_D(void)
{
    int16_t delta = Encoder_Read(&htim5);
    s_position[3] += delta;
    return delta;
}

/**
  * @brief  获取指定电机的累计位置
  * @param  motor_id 电机标识（ENCODER_MOTOR_A/B/C/D）
  * @retval 累计脉冲总数（带方向）
  */
int32_t GetEncoder_Position(EncoderMotorID motor_id)
{
    if (motor_id > ENCODER_MOTOR_D) return 0;
    return s_position[motor_id] / 100;
}

/**
  * @brief  复位所有编码器位置计数器
  */
void Encoder_ResetAll(void)
{
    for (int i = 0; i < 4; i++) {
        s_position[i] = 0;
    }
}

/* Private functions --------------------------------------------------------*/

/**
  * @brief  读取单个编码器的脉冲增量（内部使用）
  * @param  htim 定时器句柄指针
  * @retval 本次脉冲增量（自动处理计数器溢出）
  */
static int16_t Encoder_Read(TIM_HandleTypeDef *htim)
{
    int16_t count = (int16_t)(__HAL_TIM_GET_COUNTER(htim));
    __HAL_TIM_SET_COUNTER(htim, 0);
    return count;
}

/************************ (C) COPYRIGHT [公司/作者] *****END OF FILE****/