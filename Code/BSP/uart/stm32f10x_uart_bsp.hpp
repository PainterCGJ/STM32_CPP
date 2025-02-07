#pragma once
#include "stm32f10x.h"
#include <cstdio>
#include <queue>
#include "sys_bsp.hpp"
#include "rtos_bsp.hpp"
#include "priority.h"

extern "C"
{
    void USART1_IRQHandler(void);
    void USART2_IRQHandler(void);
    void USART3_IRQHandler(void);
    int std::fputc(int ch, std::FILE *f);
}
class Uart_Dev
{
public:
    friend void USART1_IRQHandler(void);
    friend void USART2_IRQHandler(void);
    friend void USART3_IRQHandler(void);
    friend int std::fputc(int ch, std::FILE *f);

    enum class UartID
    {
        Uart1,
        Uart2,
        Uart3,

        Degug_port,
        U_NUM,
    };

    typedef struct
    {
        USART_TypeDef *USARTx;

        GPIO_TypeDef *tx_port;
        uint32_t tx_pin;
        uint32_t tx_port_rcc;

        GPIO_TypeDef *rx_port;
        uint32_t rx_pin;
        uint32_t rx_port_rcc;
    } Uart_info;

    Uart_Dev(Uart_info info, uint32_t baudrate, uint16_t rx_max_size) : __rx_queue(rx_max_size)
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        uint8_t NVIC_IRQChannel;
        uint8_t priority;

        if (!__bsp_init_flag)
        {
            __bsp_init();
        }
        __dev_info = info;
        __rx_max_size = rx_max_size;

        switch ((int)(__dev_info.USARTx))
        {
        case (int)USART1:
        {
            NVIC_IRQChannel = USART1_IRQn;
            priority = UASRT1_PRIORITY;
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
            __index = (int)UartID::Uart1;
            break;
        }
        case (int)USART2:
        {
            NVIC_IRQChannel = USART2_IRQn;
            priority = UASRT2_PRIORITY;
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
            __index = (int)UartID::Uart2;
            break;
        }
        case (int)USART3:
        {
            NVIC_IRQChannel = USART3_IRQn;
            priority = UASRT3_PRIORITY;
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
            __index = (int)UartID::Uart3;
            break;
        }
        default:
            break;
        }
        __uart_dev[__index] = this;

        NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        USART_InitStructure.USART_BaudRate = baudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

        USART_Init(__dev_info.USARTx, &USART_InitStructure);
        USART_ITConfig(__dev_info.USARTx, USART_IT_RXNE, ENABLE);

        /* init gpio */
        RCC_APB2PeriphClockCmd(__dev_info.rx_port_rcc, ENABLE);
        RCC_APB2PeriphClockCmd(__dev_info.tx_port_rcc, ENABLE);

        GPIO_InitStructure.GPIO_Pin = __dev_info.tx_pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(__dev_info.tx_port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = __dev_info.rx_pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(__dev_info.rx_port, &GPIO_InitStructure);

        USART_Cmd(__dev_info.USARTx, ENABLE);
    }

    void send(uint8_t *data, uint16_t len)
    {
        for (uint16_t i = 0; i < len; i++)
        {
            USART_SendData(__dev_info.USARTx, data[i]);
            while (USART_GetFlagStatus(__dev_info.USARTx, USART_FLAG_TXE) == RESET)
                ;
        }
    }

    void send(uint8_t data)
    {
        USART_SendData(__dev_info.USARTx, data);
        while (USART_GetFlagStatus(__dev_info.USARTx, USART_FLAG_TXE) == RESET)
            ;
    }

    uint8_t recv()
    {
        uint8_t data;
        __rx_queue.pop(data);
        return data;
    }

    uint16_t recv_len()
    {
        return __rx_queue.size();
    }

    void set_debug()
    {
        __uart_dev[(int)UartID::Degug_port] = this;
    }

private:
    static Uart_Dev *__uart_dev[(int)UartID::U_NUM];
    static uint8_t __bsp_init_flag;

    static void __bsp_init()
    {
        __bsp_init_flag = 1;
        for (uint8_t i = 0; i < (int)UartID::U_NUM; i++)
        {
            __uart_dev[i] = nullptr;
        }
    }

    uint8_t __index;
    Uart_info __dev_info;
    RTOS::queue<uint8_t> __rx_queue;
    // std::queue<uint8_t, std::deque<uint8_t, OSAllocator<uint8_t>>> __rx_queue;
    // std::vector<uint8_t,OSAllocator<uint8_t>> __rx_queue;
    uint16_t __rx_max_size;
};

extern const Uart_Dev::Uart_info USART1_PA9TX_PA10RX;
extern const Uart_Dev::Uart_info USART2_PA2TX_PA3RX;
extern const Uart_Dev::Uart_info USART3_PB10TX_PB11RX;
