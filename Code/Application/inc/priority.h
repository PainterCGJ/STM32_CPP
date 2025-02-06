#pragma once
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
typedef enum
{
    OS_CALL_MAX_PRIORITY = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,
    UASRT1_PRIORITY,
    UASRT2_PRIORITY,
    UASRT3_PRIORITY,
    TIM2_PRIORITY,
    TIM3_PRIORITY,
    TIM4_PRIORITY,
}Interrupt_Priority;

typedef enum
{
    MINIMAL_PRIORITY = 0,
    COM_TASK_PRIORITY,
    MAXMAL_PRIORITY = configMAX_PRIORITIES - 1,
}Task_Priority;
