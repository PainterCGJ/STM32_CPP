#include "stm32f10x_timer_bsp.hpp"
#include "priority.h"

Timer_Dev *timer2;
Timer_Dev *timer3;

Timer_Dev::Timer_Dev(Timer_info info)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    uint8_t NVIC_IRQChannel;
    uint8_t priority;
    TIMx = info.TIMx;
    switch ((int)TIMx)
    {
    case (int)TIM2:
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        timer2 = this;
        NVIC_IRQChannel = TIM2_IRQn;
        priority = TIM2_PRIORITY;
        break;
    }
    case (int)TIM3:
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        timer3 = this;
        NVIC_IRQChannel = TIM3_IRQn;
        priority = TIM3_PRIORITY;
        break;
    }
    default:
        break;
    }
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Timer_Dev::enable()
{
    TIM_Cmd(TIMx, ENABLE); //使能定时器
}

void Timer_Dev::disable()
{
    TIM_Cmd(TIMx, DISABLE); //关闭定时器
}

extern "C"
{

    void TIM2_IRQHandler(void)
    {
        if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
        {
            timer2->isr_handle(timer2->p_arg);
            // FreeRTOSRunTimeTicks++;
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
