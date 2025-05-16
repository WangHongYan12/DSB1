/**
 * uart2_motor_frame.c
 * -------------------------------------------------------------
 * UART2 电机控制 11 字节帧解析（“# ... !”协议，无校验和，**不回传**）。
 *
 * 帧格式（索引 0 – 10）：
 *   [0]        0x23  '#'                    —— 帧头
 *   [1]~[4]    4 路目标速度  int8           —— 1 字节/路，取值 -128 ~ 127
 *   [5]        Ctrl 控制字                  —— bit0 = 1 启用梯形加减速
 *   [6]~[9]    4 路角度环速度输出 int8      —— 1 字节/路
 *   [10]       0x21  '!'                    —— 帧尾
 *
 * 解析成功后更新：
 *   uart_set_speed[4]       —— 目标速度
 *   uart_angle_velocity[4]  —— 角度环速度输出
 *   trapezoidEnabled        —— 梯形加减速使能
 */

#include "uart2_motor_frame.h"
#include "usart.h"
#include "../motor/ax_encoder.h"
#include <stdint.h>
#include <stdbool.h>

/* --------------------------- 协议常量 --------------------------- */
#define FRAME_LEN       11u          /* 整帧长度 */
#define FRAME_HEAD      0x23u        /* '#': 帧头 */
#define FRAME_TAIL      0x21u        /* '!': 帧尾 */
#define CTRL_INDEX      5u           /* Ctrl 字节索引 */
#define ANGLE_VEL_INDEX 6u           /* 角速度起始索引 */
#define TAIL_INDEX      (FRAME_LEN - 1u)

/* --------------------------- 状态机枚举 ------------------------- */
typedef enum {
    RX_WAIT_HEAD = 0,  /* 等待帧头 */
    RX_RECV_DATA,      /* 接收数据区 */
    RX_WAIT_TAIL       /* 等待帧尾 */
} RxState_t;

/* --------------------------- 静态变量 --------------------------- */
static volatile uint8_t  rxByte;              /* 单字节中断缓冲 */
static uint8_t           rxBuf[FRAME_LEN];    /* 帧缓存 */
static uint8_t           rxIndex  = 0;        /* 当前写入位置 */
static RxState_t         rxState  = RX_WAIT_HEAD;

/* --------------------------- 公共输出 --------------------------- */
volatile int8_t uart_set_speed[4]      = {0}; /* 目标速度 */
volatile int8_t uart_angle_velocity[4] = {0}; /* 角度环速度输出 */
volatile bool   trapezoidEnabled       = false;/* 梯形加减速使能 */

/* --------------------------- 内部函数声明 ----------------------- */
static void parseFrame(const uint8_t *buf);
static inline void restartRxIT(void)
{
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&rxByte, 1);
}

/* =================================================================
 * API
 * ===============================================================*/
void MotorFrame_UART2_Init(void)
{
    restartRxIT();
}

/* --- 序列检测器的静态变量 --------------------------------------------- */
static uint8_t seqBuf[3] = {0};   /* 循环缓冲记录最近 3 个字节 */
static uint8_t seqPos    = 0;     /* 下一个写入位置 (0‥2)     */

void MotorFrame_UART2_RxCallback(void)
{
    uint8_t byte = rxByte;

    /* -----------------------------------------------------
     * 1) 先做 "!@!" 检测（与帧状态机解耦，任何状态均可触发）
     * ---------------------------------------------------*/
    seqBuf[seqPos] = byte;
    seqPos = (seqPos + 1U) % 3U;

    if ((seqBuf[(seqPos + 0U) % 3U] == '!') &&
        (seqBuf[(seqPos + 1U) % 3U] == '@') &&
        (seqBuf[(seqPos + 2U) % 3U] == '!'))
    {
        Encoder_ResetAll();          /* <<< 立即复位编码器  */
        /* 重新清空检测器，防止后续字节误触发 */
        seqBuf[0] = seqBuf[1] = seqBuf[2] = 0;
        seqPos    = 0;
    }

    /* -----------------------------------------------------
     * 2) 原有帧解析状态机 —— 完全保持原样
     * ---------------------------------------------------*/
    switch (rxState)
    {
        case RX_WAIT_HEAD:
            if (byte == FRAME_HEAD) {
                rxBuf[0] = byte;
                rxIndex  = 1;
                rxState  = RX_RECV_DATA;
            }
            break;

        case RX_RECV_DATA:
            rxBuf[rxIndex] = byte;
            if (++rxIndex > (TAIL_INDEX - 1u)) {
                rxState = RX_WAIT_TAIL;        /* 已收完数据区，等待帧尾 */
            }
            break;

        case RX_WAIT_TAIL:
            if (byte == FRAME_TAIL) {
                rxBuf[TAIL_INDEX] = byte;
                parseFrame(rxBuf);             /* 成功解析 */
            }
            rxState = RX_WAIT_HEAD;            /* 重置，无论成功失败 */
            break;

        default:
            rxState = RX_WAIT_HEAD;
            break;
    }

    restartRxIT();   /* 维持原来的重新启动接收中断 */
}

/* =================================================================
 * 内部函数
 * ===============================================================*/
static void parseFrame(const uint8_t *buf)
{
    /* -------- 目标速度解析 (bytes 1~4) -------- */
    for (uint8_t i = 0; i < 4; ++i) {
        uart_set_speed[i] = (int8_t)buf[1u + i];
    }

    /* -------- Ctrl 位 (byte 5) -------- */
    trapezoidEnabled = (buf[CTRL_INDEX] & 0x01u) != 0u;

    /* -------- 角度环速度解析 (bytes 6~9) -------- */
    for (uint8_t i = 0; i < 4; ++i) {
        uart_angle_velocity[i] = (int8_t)buf[ANGLE_VEL_INDEX + i];
    }
}