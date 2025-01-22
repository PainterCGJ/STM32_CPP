#pragma once
#include "stm32f10x.h"
// #include "stm32f10x_tim.h"
#include <cstdio>
#include "sys_bsp.hpp"

class Timer_Dev
{
public:
    typedef struct
    {
        TIM_TypeDef *TIMx;
        void (*isr_handle)(void *p_arg);
        void *p_arg;
        TIM_TimeBaseInitTypeDef init_str;
    } Timer_info;
    void (*isr_handle)(void *p_arg);
    void *p_arg;
    Timer_Dev(Timer_info info);
    void enable();
    void disable();

private:
    TIM_TypeDef *TIMx;
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
