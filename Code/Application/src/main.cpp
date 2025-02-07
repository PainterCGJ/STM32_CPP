#include <cstdio>
#include "main.h"

using namespace RTOS;
 void timer_callback(void *arg)
{
	(*((int *)arg))++;
	printf("%d\r\n", (*((int *)arg)));
}
int a = 0;
Timer_Dev::Timer_info init_info = {
	TIM2,
	timer_callback,
	&a,
};

void com_task(void* parg)
{
	Uart_Dev com(USART1_PA9TX_PA10RX, 115200, 100);
	com.set_debug();
	printf("hello world\r\n");
	uint8_t rx;
	while (1)
	{
		while (com.recv_len())
		{
			rx = com.recv();
			com.send(&rx, 1);
		}
		// printf("com\r\n");
		delay_ms(100);
		
	}
}
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	systick_init();
	Thread COM(com_task, "com", 1);
	// Uart_Dev com(USART1_PA9TX_PA10RX, 115200, 100);
	// com.set_debug();
	//xTaskCreate(com_task, "com", 128, NULL, 10, handler);
//	com.send((uint8_t *)"Hello World!\r\n", 14);
//	Timer_Dev timer(init_info, 2000000);
//	timer.enable();
	
	RTOS::os_start_scheduler();
	while (1)
	{
		
	}
}
void vApplicationStackOverflowHook(TaskHandle_t xTask,
								   char *pcTaskName)
{
}
