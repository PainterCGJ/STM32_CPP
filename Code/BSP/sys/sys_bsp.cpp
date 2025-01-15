#include "sys_bsp.hpp"

// #include <limits>
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI


extern "C"
{
  #pragma import(__use_no_semihosting) // 确保没有从 C 库链接使用半主机的函数
  //#include "stdio.h"
  void _sys_exit(int x) //定义 _sys_exit() 以避免使用半主机模式
  {
    x = x;
  }
  struct __FILE // 标准库需要的支持函数
  {
    int handle;
  };
  /* FILE is typedef ’ d in stdio.h. */
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
//关闭所有中断
void INTX_DISABLE(void)
{
  __ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
  __ASM volatile("cpsie i");
}
//设置栈顶地址
//addr:栈顶地址
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

// 自定义内存分配器


