/* uart2_dma_tx.h — public interface for uart2_dma_tx.c
 * ----------------------------------------------------
 * Provides a simple API to send a 42‑byte framed packet over USART2 using DMA.
 */

#ifndef UART2_DMA_TX_H
#define UART2_DMA_TX_H

#include "main.h"   /* for UART_HandleTypeDef, HAL defs */
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Packet length in bytes (header + payload + tail) */


/**
 * @brief  Attempt to assemble and send one data packet via USART2 DMA.
 *         If the previous DMA transfer is still ongoing, the call returns
 *         immediately without sending.
 */
void Uart2DmaSendPacket(void);

#ifdef __cplusplus
}
#endif

#endif /* UART2_DMA_TX_H */
