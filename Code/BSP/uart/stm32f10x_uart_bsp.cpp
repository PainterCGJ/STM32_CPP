#include "stm32f10x_uart_bsp.hpp"
#include "sys_bsp.hpp"
#include "priority.h"

Uart_Dev *Uart_Dev::__uart_dev[(int)UartID::U_NUM];
uint8_t Uart_Dev::__bsp_init_flag = 0;

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

extern "C"
{
    void USART1_IRQHandler(void)
    {
        if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
        {
            uint8_t data = USART_ReceiveData(USART1);
            Uart_Dev::__uart_dev[(int)Uart_Dev::UartID::Uart1]->__rx_queue.push(data);
        }
    }

    void USART2_IRQHandler(void)
    {
        if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
        {
            uint8_t data = USART_ReceiveData(USART2);
            Uart_Dev::__uart_dev[(int)Uart_Dev::UartID::Uart2]->__rx_queue.push(data);
        }
    }

    void USART3_IRQHandler(void)
    {
        if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
        {
            uint8_t data = USART_ReceiveData(USART3);
            Uart_Dev::__uart_dev[(int)Uart_Dev::UartID::Uart3]->__rx_queue.push(data);
        }
    }
}

int std::fputc(int ch, std::FILE *f)
{
    Uart_Dev::__uart_dev[(int)Uart_Dev::UartID::Degug_port]->send(ch);
    return ch;
}
