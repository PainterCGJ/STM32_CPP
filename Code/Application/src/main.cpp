#include <cstdio>
#include "main.h"
// #include <list>
// #include <initializer_list>
using namespace RTOS;
void timer_callback(void *arg)
{
	(*((int *)arg))++;
	printf("%d\r\n", (*((int *)arg)));
}

void com_task(void *parg)
{
	Uart_Dev com(USART1_PA9TX_PA10RX, 115200, 100, 200);
	com.set_debug();
	printf("hello world\r\n");
	uint8_t rx;
	while (1)
	{
		//收到什么就返回什么
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
class COM : public Thread
{
public:
	COM(semaphore<binary_semaphore_tag> &sme, event &evt) : Thread("COM", 3, 128), _sme(sme), _evt(evt)
	{
		// join();
	}

	virtual void thread_code() override
	{
		Uart_Dev com(USART1_PA9TX_PA10RX, 115200, 100, 200);
		com.set_debug();
		
		_sme.give();
		printf("hello world\r\n");
		uint8_t rx;
		delay_ms(20);
		OS_EventBits bits = _evt.sync(0x02,0x01,0);
		printf("COM:%d\r\n",bits);
		while (1)
		{
			//收到什么就返回什么
			while (com.recv_len())
			{
				rx = com.recv();
				com.send(&rx, 1);
			}
			delay_ms(100);
		}
	}

private:
	semaphore<binary_semaphore_tag> &_sme;
	event &_evt;
};
class TEST : public Thread
{
public:
	TEST(semaphore<binary_semaphore_tag> &sme, event &evt) : Thread("TEST", 4, 128), _sme(sme), _evt(evt)
	{
		join();
	}
	virtual void thread_code() override
	{
		_sme.take();
		OS_EventBits bits = _evt.sync(0x01,0x02);
		printf("TEST:%d\r\n",bits);
		while (1)
		{
			printf("running...\r\n");
			delay_ms(1000);
		}
	}

private:
	semaphore<binary_semaphore_tag> &_sme;
	event &_evt;
};

typedef struct
{
	int e;
	int b;
} _str;
void rtos_main(void)
{
	//创建串口线程
	// Thread COM1(com_task, "com", 1, 100);
	semaphore<binary_semaphore_tag> a;
	event evt;
	TEST test(a, evt);
	COM comth(a, evt);
	comth.join();
	
	// sizeof(COM);
	while (1)
	{
	}
}
void vApplicationStackOverflowHook(TaskHandle_t xTask,
								   char *pcTaskName)
{
}
