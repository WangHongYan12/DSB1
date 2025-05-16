/**
 * uart2_motor_frame.h
 * -------------------------------------------------------------
 * UART2 电机控制帧解析器（11 字节，无校验和）
 *
 * 帧格式（下标 0~10）：
 *   [0]        0x23 '#'                —— 帧头
 *   [1]~[4]    4 路目标速度 int8      —— 1 字节
 *   [5]        Ctrl 控制字             —— bit0 = 1 启用梯形加减速
 *   [6]~[9]    4 路角度环速度输出 int8 —— 1 字节
 *   [10]       0x21 '!'                —— 帧尾
 *
 * API：
 *   MotorFrame_UART2_Init()        —— 启动单字节中断接收
 *   MotorFrame_UART2_RxCallback()  —— 在 USART2_IRQHandler 中调用
 *
 * 全局输出：
 *   uart_set_speed[4]      —— 目标速度
 *   uart_angle_velocity[4] —— 角度环速度输出
 *   trapezoidEnabled       —— 梯形加减速使能
 */

#ifndef UART2_MOTOR_FRAME_H
#define UART2_MOTOR_FRAME_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化：启动 UART2 单字节中断接收 */
void MotorFrame_UART2_Init(void);

/* 串口中断回调：在 USART2_IRQHandler 中调用 */
void MotorFrame_UART2_RxCallback(void);

/* 数据输出 ----------------------------------------------------------------*/
extern volatile int8_t uart_set_speed[4];       /* 目标速度 */
extern volatile int8_t uart_angle_velocity[4];  /* 角度环速度输出 */
extern volatile bool    trapezoidEnabled;       /* 梯形加减速使能 */

#ifdef __cplusplus
}
#endif

#endif /* UART2_MOTOR_FRAME_H */
