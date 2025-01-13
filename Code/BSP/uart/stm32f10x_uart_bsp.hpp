#pragma once
#include "stm32f10x.h"
#include <cstdio>
#include <queue>
#include <vector>
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
    Uart_Dev(Uart_info info, uint32_t baudrate, uint16_t rx_max_size);
    void send(uint8_t *data, uint16_t len);
    uint8_t recv();
    uint16_t recv_len();
    void isr_handler(uint8_t data);
private:
    uint8_t __if_init;
    Uart_info __dev_info;
    // std::queue<uint8_t> __rx_queue;
    std::vector<uint8_t> __rx_queue;
    uint16_t __rx_max_size;
};

extern const Uart_Dev::Uart_info USART1_PA9TX_PA10RX;
extern const Uart_Dev::Uart_info USART2_PA2TX_PA3RX;
extern const Uart_Dev::Uart_info USART3_PB10TX_PB11RX;
