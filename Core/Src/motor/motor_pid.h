/**
 * @file motor_pid.h
 * @brief 四电机PID速度控制器头文件
 * @version 1.0
 * @date 2023-XX-XX
 *
 * @note 需配套motor_pid.c使用，依赖ax_motor.h和ax_encoder.h
 */

#ifndef __MOTOR_PID_H
#define __MOTOR_PID_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 电机标识枚举 ------------------------------------------------------------*/
typedef enum {
    MOTOR_A,  ///< 电机A（如左前轮）
    MOTOR_B,  ///< 电机B（如右前轮）
    MOTOR_C,  ///< 电机C（如左后轮）
    MOTOR_D   ///< 电机D（如右后轮）
} MotorID;

/* 外部可访问变量 --------------------------------------------------------*/
extern int target_speeds[4];  ///< 目标速度数组（索引对应MotorID）
extern int pwm_outputs[4];    ///< PWM输出数组（±OUTPUT_LIMIT）
extern int real_speeds[4];    ///< 四个电机的实际速度（需由编码器读取）
/* 函数声明 --------------------------------------------------------------*/

/**
 * @brief 初始化PID控制器
 * @note 必须在系统启动时调用一次
 */
void PID_Init(void);

/**
 * @brief 电机速度PID控制任务
 * @note 需在定时器中断中周期性调用（建议1kHz）
 */
void Motor_Speed_PID_Control(void);

/**
 * @brief 设置单个电机目标速度
 * @param id 电机标识（MOTOR_A~MOTOR_D）
 * @param speed 目标速度值
 */
static inline void Set_Target_Speed(MotorID id, int speed) {
    target_speeds[id] = speed;
}

/**
 * @brief 获取单个电机当前PWM输出
 * @param id 电机标识（MOTOR_A~MOTOR_D）
 * @return PWM输出值（±OUTPUT_LIMIT）
 */
static inline int Get_Motor_Output(MotorID id) {
    return pwm_outputs[id];
}

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_PID_H */