#include "sys_bsp.hpp"

// #include <limits>
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI


extern "C"
{
  #pragma import(__use_no_semihosting) // ȷ��û�д� C ������ʹ�ð������ĺ���
  //#include "stdio.h"
  void _sys_exit(int x) //���� _sys_exit() �Ա���ʹ�ð�����ģʽ
  {
    x = x;
  }
  struct __FILE // ��׼����Ҫ��֧�ֺ���
  {
    int handle;
  };
  /* FILE is typedef �� d in stdio.h. */
  extern FILE __stdout;
  void _ttywrch(int x)
  {
    x = x;
  }
}

void WFI_SET(void)
{
  __ASM volatile("wfi");
}
//�ر������ж�
void INTX_DISABLE(void)
{
  __ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
  __ASM volatile("cpsie i");
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr)
{
  MSR MSP, r0 //set Main Stack value
               BX r14
}

void *operator new(size_t size) throw(std::bad_alloc)
{
 return pvPortMalloc(size);
}

void *operator new[](size_t size) throw(std::bad_alloc)
{
 return pvPortMalloc(size);
}

void operator delete(void *pointer) throw()
{
 vPortFree(pointer);
}

void operator delete[](void *pointer) throw()
{
 vPortFree(pointer);
}

// �Զ����ڴ������


