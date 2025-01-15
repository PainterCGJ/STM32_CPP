#include <cstdio>
#include "main.h"
#include <vector>
// #include <utility>

int main(void)
{
	Uart_Dev com(USART1_PA9TX_PA10RX, 115200, 100);
	com.set_debug();
	// com.send((uint8_t *)"Hello World!\r\n", 14);
	printf("Hello World!\r\n");
	uint8_t rx;
	while (1)
	{
		if(com.recv_len())
		{
			rx = com.recv();
			com.send(&rx,1);
		}
	}
}
void vApplicationStackOverflowHook(TaskHandle_t xTask,
								   char *pcTaskName)
{
}
