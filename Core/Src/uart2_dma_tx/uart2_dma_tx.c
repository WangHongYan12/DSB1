// uart2_dma_tx.c — helper for framed USART2 DMA packet transmission
// ----------------------------------------------------------------------------
// Packet format (little‑endian):
//   Header      : 1 byte  -> '#'
//   Target speed: 4×int16 -> uart_set_speed[4]
//   Odometer    : 4×int32 -> s_position[4]
//   Real speed  : 4×int32 -> real_speeds[4]
//   Tail        : 1 byte  -> '!'
// Total length  : 42 bytes
// ----------------------------------------------------------------------------
// Usage:
//   • CubeMX 生成 USART2 (DMA1_Channel7) 与 TIM8 更新中断。
//   • 将本文件加入工程。
//   • 在 TIM8 更新回调中调用 Uart2DmaSendPacket();
//     每 20 Hz 会封装并发送一次以上数据帧。
// ----------------------------------------------------------------------------

#include "main.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "F:\Project\DSB1\Core\Src\motor\ax_encoder.h"
#include "usart.h"
#include <stdio.h>
#include "F:\Project\DSB1\Core\Src\motor_frame\uart2_motor_frame.h"
#include "F:\Project\DSB1\Core\Src\motor\ax_motor.h"
#include "F:\Project\DSB1\Core\Src\motor\motor_pid.h"


#define TX_PKT_LEN  42                         // 1 + 8 + 16 + 16 + 1

static uint8_t txBuf[TX_PKT_LEN];              // DMA 发送缓冲区
static volatile bool txBusy = false;           // DMA 正忙标志

/* 封装 42‑byte 数据帧到 txBuf */
static void PreparePacket(void)
{
    uint8_t *p = txBuf;
    *p++ = '#';                                // 帧头

    // 1. 目标速度 4×int16
    for (int i = 0; i < 4; ++i) {
        int16_t v = uart_set_speed[i];
        memcpy(p, &v, sizeof(int16_t));
        p += sizeof(int16_t);
    }

    // 2. 里程计 4×int32
    for (int i = 0; i < 4; ++i) {
        int32_t s = GetEncoder_Position(i);
        memcpy(p, &s, sizeof(int32_t));
        p += sizeof(int32_t);
    }

    // 3. 实际速度 4×int32 (若 real_speeds 为 32 位；调整以匹配实际类型)
    for (int i = 0; i < 4; ++i) {
        int32_t r = real_speeds[i];
        memcpy(p, &r, sizeof(int32_t));
        p += sizeof(int32_t);
    }

    *p++ = '!';                                // 帧尾

}

/**
  * @brief  准备ASCII格式数据包
  * @note   帧格式: #SPD1,SPD2,SPD3,SPD4,POS1,POS2,POS3,POS4,REAL1,REAL2,REAL3,REAL4!
  *         示例: #1500,-200,0,800,12345,-678,0,99999,1490,-195,5,810!
  */

/* 若 DMA 空闲则封装并启动一次发送 */
void Uart2DmaSendPacket(void)
{
    if (txBusy) {
        return;                                // 上一次尚未完成
    }

    PreparePacket();

    if (HAL_UART_Transmit_DMA(&huart2, txBuf, TX_PKT_LEN) == HAL_OK) {
        txBusy = true;
    }
}

/* HAL 回调：发送完成 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        txBusy = false;
    }
}

/* HAL 回调：发送错误 (可选复位) */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        txBusy = false;
    }
}
