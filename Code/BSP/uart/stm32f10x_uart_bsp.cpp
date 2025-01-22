#include "stm32f10x_uart_bsp.hpp"
#include "sys_bsp.hpp"
#include "priority.h"

static Uart_Dev *usart1;
static Uart_Dev *usart2;
static Uart_Dev *usart3;
static Uart_Dev *debug_port;

const Uart_Dev::Uart_info USART1_PA9TX_PA10RX __attribute__((unused)) = {
    USART1,
    GPIOA,
    GPIO_Pin_9,
    RCC_APB2Periph_GPIOA,
    GPIOA,
    GPIO_Pin_10,
    RCC_APB2Periph_GPIOA,
};

const Uart_Dev::Uart_info USART2_PA2TX_PA3RX __attribute__((unused)) = {
    USART2,
    GPIOA,
    GPIO_Pin_2,
    RCC_APB2Periph_GPIOA,
    GPIOA,
    GPIO_Pin_3,
    RCC_APB2Periph_GPIOA,
};

const Uart_Dev::Uart_info USART3_PB10TX_PB11RX __attribute__((unused)) = {
    USART3,

    GPIOB,
    GPIO_Pin_10,
    RCC_APB2Periph_GPIOB,

    GPIOB,
    GPIO_Pin_11,
    RCC_APB2Periph_GPIOB,
};

Uart_Dev::Uart_Dev()
{
    __if_init = 0;
}
Uart_Dev::Uart_Dev(Uart_info info, uint32_t baudrate, uint16_t rx_max_size)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    uint8_t NVIC_IRQChannel;
    uint8_t priority;

    __dev_info = info;
    __rx_max_size = rx_max_size;

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

    switch ((int)(__dev_info.USARTx))
    {
    case (int)USART1:
    {
        NVIC_IRQChannel = USART1_IRQn;
        priority = UASRT1_PRIORITY;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        usart1 = this;
        break;
    }
    case (int)USART2:
    {
        NVIC_IRQChannel = USART2_IRQn;
        priority = UASRT2_PRIORITY;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        usart2 = this;
        break;
    }
    case (int)USART3:
    {
        NVIC_IRQChannel = USART3_IRQn;
        priority = UASRT3_PRIORITY;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        usart3 = this;
        break;
    }
    default:
        break;
    }

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

    USART_Cmd(__dev_info.USARTx, ENABLE);
    __if_init = 1;
}

void Uart_Dev::set_debug()
{
    debug_port = this;
}

void Uart_Dev::send(uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        USART_SendData(__dev_info.USARTx, data[i]);
        while (USART_GetFlagStatus(__dev_info.USARTx, USART_FLAG_TXE) == RESET)
            ;
    }
}

void Uart_Dev::send(uint8_t data)
{
    USART_SendData(__dev_info.USARTx, data);
    while (USART_GetFlagStatus(__dev_info.USARTx, USART_FLAG_TXE) == RESET)
        ;
}

void Uart_Dev::isr_handler(uint8_t data)
{
    if (__rx_queue.size() < __rx_max_size)
    {
        __rx_queue.push(data);
    }
}

uint8_t Uart_Dev::recv()
{
    uint8_t data = __rx_queue.front();
    __rx_queue.pop();
    return data;
}

uint16_t Uart_Dev::recv_len()
{
    return __rx_queue.size();
}

extern "C"
{
    void USART1_IRQHandler(void)
    {
        if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
        {
            uint8_t data = USART_ReceiveData(USART1);
            usart1->isr_handler(data);
        }
    }

    void USART2_IRQHandler(void)
    {
        if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
        {
            uint8_t data = USART_ReceiveData(USART2);
            usart2->isr_handler(data);
        }
    }

    void USART3_IRQHandler(void)
    {
        if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
        {
            uint8_t data = USART_ReceiveData(USART3);
            usart3->isr_handler(data);
        }
    }
}

int std::fputc(int ch, std::FILE *f)
{
    debug_port->send(ch);
    return ch;
}
