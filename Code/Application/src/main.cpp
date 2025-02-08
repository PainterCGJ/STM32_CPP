#include <cstdio>
#include "main.h"
// #include <thread>
using namespace RTOS;
void timer_callback(void *arg)
{
	(*((int *)arg))++;
	printf("%d\r\n", (*((int *)arg)));
}

void com_task(void *parg)
{
	Uart_Dev com(USART1_PA9TX_PA10RX, 115200, 100);
	com.set_debug();
	printf("hello world\r\n");
	uint8_t rx;
	// COM.kill();
	while (1)
	{
		while (com.recv_len())
		{
			rx = com.recv();
			com.send(&rx, 1);
		}
		delay_ms(100);
	}
}

void system_config(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	systick_init();
}
void rtos_main(void)
{
	Thread COM(com_task, "com", 1, 100);

	int a = 10;
	list<int> li;
	li.push_back(a);
	a=20;
	li.push_back(a);
	li.print();
	// int &b=li.back();
	printf("b=%d\r\n",li.back());
	printf("b=%d\r\n",li.front());


	while(1)
	{
		//delay_ms(1000);
	}
	// Timer_Dev::Timer_info init_info = {
	// 	TIM2,
	// 	timer_callback,
	// 	&a,
	// };
	// Timer_Dev timer(init_info, 2000000);
	// timer.enable();
	// RTOS::os_start_scheduler();
}
void vApplicationStackOverflowHook(TaskHandle_t xTask,
								   char *pcTaskName)
{
}
