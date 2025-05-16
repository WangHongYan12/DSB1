/* speed_ramp.c -------------------------------------------------*
 * 用户仅需修改 2 个参数:
 *   1) TIMER_FREQ_HZ   —— 定时器真正的中断频率 (Hz)
 *   2) ACC_RPM_PER_SEC —— 允许的最大加速度 (RPM / s)
 * 其他常量全部由这两项自动推导，保持整数运算。
 *---------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include "F:\\Project\\DSB1\\Core\\Src\\motor_frame\\uart2_motor_frame.h"
#include "F:\\Project\\DSB1\\Core\\Src\\motor\\motor_pid.h"

/*=================== 用户可调宏 ===================*/
#define TIMER_FREQ_HZ      10      /* 周期（毫秒） */
#define ACC_RPM_PER_SEC    1         /* 每秒最多增加的 RPM */
/*==================================================*/

/*------- 内部派生常量 (勿改) -----------------------*/
#define SCALE_MRPM   1000                                    /* 1 RPM = 1000 mRPM */
#define STEP_MRPM  ((ACC_RPM_PER_SEC * SCALE_MRPM            \
                     + TIMER_FREQ_HZ / 2) / TIMER_FREQ_HZ)   /* 四舍五入取整 */
/*--------------------------------------------------*/

/* 高分辨率目标速度 (单位: mRPM) --------------------*/
static int32_t target_mrpm[4] = {0};

/*=================== 斜坡更新 =====================*/
void SpeedRamp_Update(void)
{
    for (uint8_t i = 0; i < 4; ++i)
    {
        /* 把 UART 指令从 RPM → mRPM */
        int32_t goal_m = (int32_t)uart_set_speed[i] * SCALE_MRPM;
        int32_t cur_m  = target_mrpm[i];

        if (trapezoidEnabled)                    /* 梯形斜率限制 */
        {
            int32_t diff = goal_m - cur_m;

            if      (diff >  STEP_MRPM) cur_m += STEP_MRPM;
            else if (diff < -STEP_MRPM) cur_m -= STEP_MRPM;
            else                        cur_m  = goal_m;      /* 收敛 */
        }
        else                                     /* 阶跃 */
        {
            cur_m = goal_m;
        }

        target_mrpm[i]   = cur_m;                     /* 保存高分辨率 */
        target_speeds[i] = (int16_t)(cur_m / SCALE_MRPM); /* 输出整 RPM 供 PID */
    }
}
