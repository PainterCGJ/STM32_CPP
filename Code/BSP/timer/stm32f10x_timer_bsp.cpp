#include "stm32f10x_timer_bsp.hpp"
#include "priority.h"

Timer_Dev *timer2;
Timer_Dev *timer3;
Timer_Dev *timer4;

uint8_t Timer_Dev::_bsp_init_flag = 0;
Timer_Dev *Timer_Dev::_timer_dev[(int)Timer_Dev::TimerID::T_NUM];

extern "C"
{

    void TIM2_IRQHandler(void)
    {
        if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
        {
            Timer_Dev::_timer_dev[(uint8_t)Timer_Dev::TimerID::TIMER2]->isr_handle();
            // FreeRTOSRunTimeTicks++;
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
