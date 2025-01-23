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
	// TIM_TimeBaseStructure.TIM_Period = 1000; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	// TIM_TimeBaseStructure.TIM_Prescaler = 2; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	// TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	// TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	
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
