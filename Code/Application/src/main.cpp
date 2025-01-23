#include <cstdio>
#include "main.h"
// #include <utility>

void timer_callback(void* arg)
{
	(*((int*)arg))++;
	printf("%d\r\n",(*((int*)arg)));
}
int a=0;
	Timer_Dev::Timer_info init_info={
		TIM2,
		timer_callback,
		&a,
	};
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Uart_Dev com(USART1_PA9TX_PA10RX, 115200, 100);
	com.set_debug();
	com.send((uint8_t *)"Hello World!\r\n", 14);
	// TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// TIM_TimeBaseStructure.TIM_Period = 1000; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	// TIM_TimeBaseStructure.TIM_Prescaler = 2; //设置用来作为TIMx时钟频率除数的预分频值
	// TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	// TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	
	// Timer_Dev timer(init_info,TIM_TimeBaseStructure);
	Timer_Dev timer(init_info,2000000);
	timer.enable();
	uint8_t rx;
	while (1)
	{
		if (com.recv_len())
		{
			rx = com.recv();
			com.send(&rx, 1);
		}
	}
}
void vApplicationStackOverflowHook(TaskHandle_t xTask,
								   char *pcTaskName)
{
}
