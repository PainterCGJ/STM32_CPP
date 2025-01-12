#include <cstdio>
#include "main.h"
int main(void)
{
	Uart_Dev com(USART1_PA9TX_PA10RX, 115200);
	com.send((uint8_t *)"Hello World!\r\n", 14);
	while (1)
	{
	}
}
void vApplicationStackOverflowHook(TaskHandle_t xTask,
								   char *pcTaskName)
{
}
