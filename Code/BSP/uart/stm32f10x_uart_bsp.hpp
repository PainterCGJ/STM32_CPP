#pragma once
#include "stm32f10x.h"
#include <cstdio>
class Uart_Dev
{
public:
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
    Uart_Dev();
    Uart_Dev(Uart_info info, uint32_t baudrate);
    void send(uint8_t *data, uint16_t len);

private:
    uint8_t if_init;
    Uart_info dev_info;
};

extern const Uart_Dev::Uart_info USART1_PA9TX_PA10RX;
extern const Uart_Dev::Uart_info USART2_PA2TX_PA3RX;
extern const Uart_Dev::Uart_info USART3_PB10TX_PB11RX;
