#pragma once
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include <cstdio>
#include "sys_bsp.hpp"
#include "priority.h"
extern "C"
{
    void TIM2_IRQHandler(void);
    void TIM3_IRQHandler(void);
    void TIM4_IRQHandler(void);
}

class Timer_Dev
{
public:
    enum class TimerID
    {
        TIMER0,
        TIMER1,
        TIMER2,
        TIMER3,
        TIMER4,

        T_NUM,
    };

    typedef struct
    {
        TIM_TypeDef *TIMx;
        void (*isr_callback)(void *p_arg);
        void *p_arg;
    } Timer_info;

    Timer_Dev(Timer_info info, TIM_TimeBaseInitTypeDef init_str)
    {
        __timer_init(info, init_str);
    }
    Timer_Dev(Timer_info info, uint32_t us)
    {
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        uint32_t fac = us / 65535 + 1;

        //定时器初始化
        TIM_TimeBaseStructure.TIM_Period = us / fac - 1;            //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
        TIM_TimeBaseStructure.TIM_Prescaler = 72 * fac;             //设置用来作为TIMx时钟频率除数的预分频值
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式

        __timer_init(info, TIM_TimeBaseStructure);
    }

    void enable()
    {
        TIM_Cmd(__TIMx, ENABLE); //使能定时器
    }

    void disable()
    {
        TIM_Cmd(__TIMx, DISABLE); //关闭定时器
    }

    friend void TIM2_IRQHandler(void);
    friend void TIM3_IRQHandler(void);
    friend void TIM4_IRQHandler(void);

private:
    TIM_TypeDef *__TIMx;
    void (*__isr_callback)(void *__p_arg);
    void *__p_arg;
    uint8_t __index;
    static uint8_t __bsp_init_flag;
    static Timer_Dev *__timer_dev[(int)TimerID::T_NUM];

    void __isr_handle()
    {
        if (__isr_callback != nullptr)
        {
            __isr_callback(__p_arg);
        }
    }
    void __timer_init(Timer_info info, TIM_TimeBaseInitTypeDef init_str)
    {
        if (!__bsp_init_flag)
        {
            __bsp_init();
        }
        NVIC_InitTypeDef NVIC_InitStructure;
        uint8_t NVIC_IRQChannel;
        uint8_t priority;
        __isr_callback = info.isr_callback;
        __p_arg = info.p_arg;
        __TIMx = info.TIMx;
        switch ((int)__TIMx)
        {
        case (int)TIM2:
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
            NVIC_IRQChannel = TIM2_IRQn;
            priority = TIM2_PRIORITY;
            __index = (uint8_t)TimerID::TIMER2;
            break;
        }
        case (int)TIM3:
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
            NVIC_IRQChannel = TIM3_IRQn;
            priority = TIM3_PRIORITY;
            __index = (uint8_t)TimerID::TIMER3;
            break;
        }
        case (int)TIM4:
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
            NVIC_IRQChannel = TIM4_IRQn;
            priority = TIM4_PRIORITY;
            __index = (uint8_t)TimerID::TIMER4;
            break;
        }
        default:
            break;
        }

        __timer_dev[__index] = this;

        TIM_TimeBaseInit(__TIMx, &init_str);
        TIM_ClearITPendingBit(__TIMx, TIM_IT_Update);

        TIM_ITConfig(__TIMx, TIM_IT_Update, ENABLE);

        NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }

    static void __bsp_init()
    {
        __bsp_init_flag = 1;
        for (uint8_t i = 0; i < (int)TimerID::T_NUM; i++)
        {
            __timer_dev[i] = nullptr;
        }
    }
};

class PWM_Dev
{
public:
    typedef struct
    {
        uint16_t channel;
    } PWM_CH_Cfg;
    PWM_Dev();

private:
};
