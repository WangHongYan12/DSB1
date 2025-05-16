#include <stdint.h>
#include "ax_motor.h"
#include "ax_encoder.h"
#include "tim.h"
#include "F:\Project\DSB1\Core\Src\motor_frame\uart2_motor_frame.h"
/**
 * @file motor_pid.c
 * @brief 四电机PID速度控制器（全整型实现）
 *
 * 功能：
 * 1. 支持四个电机（A/B/C/D）的独立PID控制
 * 2. 全整型运算，适合无FPU的MCU
 * 3. 可在定时器中断中直接调用
 */

// PID 参数和限幅配置
#define INTEGRAL_LIMIT   100000   ///< 积分限幅值（放大100倍存储）
#define OUTPUT_LIMIT     1000     ///< PWM输出限幅值（±1000）

/**
 * @brief 电机标识枚举
 */
typedef enum {
    MOTOR_A,  ///< 电机A（如左前轮）
    MOTOR_B,  ///< 电机B（如右前轮）
    MOTOR_C,  ///< 电机C（如左后轮）
    MOTOR_D   ///< 电机D（如右后轮）
} MotorID;

/**
 * @brief PID状态结构体（全整型）
 */
typedef struct {
    int integral;      ///< 积分项（实际值 = integral / 100）
    int prev_error;    ///< 上一次速度误差
} PID_State;

/**
 * @brief PID参数结构体（全整型）
 * @note 所有参数实际值 = 存储值/100（保留2位小数）
 */
typedef struct {
    int Kp;           ///< 比例系数（实际值 = Kp / 100）
    int Ki;           ///< 积分系数（实际值 = Ki / 100）
    int Kd;           ///< 微分系数（实际值 = Kd / 100）
} PID_Params;

// 全局变量
static PID_State motor_states[4];  ///< 四个电机的PID状态
static PID_Params pid_params = {   ///< 统一PID参数（可改为数组实现独立参数）
        .Kp = 5500,  // 实际值 = 60.00
        .Ki = 800,   // 实际值 = 5.00
        .Kd = 00    // 实际值 = 2.00
};

int target_speeds[4];  ///< 四个电机的目标速度（单位：编码器计数值）
int real_speeds[4];    ///< 四个电机的实际速度（需由编码器读取）
int pwm_outputs[4];    ///< 四个电机的PWM输出值（±OUTPUT_LIMIT）

/* 私有函数声明 */
static int PID_Control(MotorID id, int setpoint, int real_speed);

/**
 * @brief 初始化PID控制器
 * @note 上电或急停后需调用此函数清零历史状态
 */
void PID_Init(void) {
    HAL_TIM_Base_Start_IT(&htim6);  // 启动TIM6中断
    for (int i = 0; i < 4; i++) {
        motor_states[i].integral = 0;
        motor_states[i].prev_error = 0;
        target_speeds[i] = 0;
        real_speeds[i] = 0;
        pwm_outputs[i] = 0;
    }
}

/**
 * @brief PID控制计算（单电机）
 * @param id 电机标识（MOTOR_A~MOTOR_D）
 * @param setpoint 目标速度
 * @param real_speed 实际速度（需与目标速度同单位）
 * @return PWM输出值（±OUTPUT_LIMIT）
 * @note 计算过程全整型，公式：output = (Kp*e + Ki*∫e + Kd*Δe)/100
 */
static int PID_Control(MotorID id, int setpoint, int real_speed) {
    PID_State *state = &motor_states[id];

    // 1. 计算误差
    int error = setpoint - real_speed;

    // 2. 积分项计算（带限幅）
    state->integral += error;
    if (state->integral > INTEGRAL_LIMIT) {
        state->integral = INTEGRAL_LIMIT;
    } else if (state->integral < -INTEGRAL_LIMIT) {
        state->integral = -INTEGRAL_LIMIT;
    }

    // 3. 微分项计算
    int derivative = error - state->prev_error;

    // 4. PID公式计算（注意系数已放大100倍）
    // output = [Kp*e + Ki*(∫e) + Kd*(Δe)] / 100
    int output = (pid_params.Kp * error +
                  pid_params.Ki * state->integral +
                  pid_params.Kd * derivative) / 100;

    // 5. 输出限幅
    if (output > OUTPUT_LIMIT) {
        output = OUTPUT_LIMIT;
    } else if (output < -OUTPUT_LIMIT) {
        output = -OUTPUT_LIMIT;
    }

    // 6. 更新误差记录
    state->prev_error = error;

    return output;
}

/**
 * @brief 更新四个电机的PID控制
 * @param target_speeds 目标速度数组（索引需对应MotorID）
 * @param real_speeds 实际速度数组（需提前通过编码器获取）
 * @param outputs PWM输出数组（用于驱动电机）
 * @note 应在控制周期固定调用（如1kHz定时器中断）
 */
void Update_Motors(const int target_speeds[4], const int real_speeds[4], int outputs[4]) {
    outputs[MOTOR_A] = PID_Control(MOTOR_A, target_speeds[MOTOR_A] + uart_angle_velocity[MOTOR_A], real_speeds[MOTOR_A]);
    outputs[MOTOR_B] = PID_Control(MOTOR_B, target_speeds[MOTOR_B] + uart_angle_velocity[MOTOR_B], real_speeds[MOTOR_B]);
    outputs[MOTOR_C] = PID_Control(MOTOR_C, target_speeds[MOTOR_C] + uart_angle_velocity[MOTOR_C], real_speeds[MOTOR_C]);
    outputs[MOTOR_D] = PID_Control(MOTOR_D, target_speeds[MOTOR_D] + uart_angle_velocity[MOTOR_D], real_speeds[MOTOR_D]);
}

/**
 * @brief 电机速度PID控制任务
 * @note 需在定时器中断中周期性调用（如1kHz）
 * 执行流程：
 * 1. 读取编码器值 -> real_speeds[]
 * 2. 计算PID输出 -> pwm_outputs[]
 * 3. 输出PWM到电机
 */
void Motor_Speed_PID_Control(void) {
    // 1. 读取实际速度（需实现GetEncoder_X()函数）
    real_speeds[MOTOR_A] = GetEncoder_A();
    real_speeds[MOTOR_B] = GetEncoder_B();
    real_speeds[MOTOR_C] = GetEncoder_C();
    real_speeds[MOTOR_D] = GetEncoder_D();

    // 2. 计算PID输出
    Update_Motors(target_speeds, real_speeds, pwm_outputs);

    // 3. 驱动电机（需实现Motor_OutPut()函数）
    Motor_OutPut(
            pwm_outputs[MOTOR_A],
            pwm_outputs[MOTOR_B],
            pwm_outputs[MOTOR_C],
            pwm_outputs[MOTOR_D]
    );
}

/********************************
 * 使用示例：
 *
 * 1. 初始化：
 *    PID_Init();
 *    target_speeds[MOTOR_A] = 1000; // 设置目标速度
 *
 * 2. 在1kHz定时器中调用：
 *    Motor_Speed_PID_Control();
 *
 * 3. 动态修改目标速度：
 *    target_speeds[MOTOR_B] = new_speed;
 ********************************/