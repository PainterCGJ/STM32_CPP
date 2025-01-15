#pragma once

#include "stm32f10x.h"
#include <cstddef>
#include <cstdint>
#include <new>
#include "FreeRTOS.h"
// 0,��֧��os
// 1,֧��os
#define SYSTEM_SUPPORT_OS 1 // ����ϵͳ�ļ����Ƿ�֧��OS
#if SYSTEM_SUPPORT_OS
#ifndef USE_OS
#define USE_OS 1 // ʹ��OS
#endif
#endif

// λ������,ʵ��51���Ƶ�GPIO���ƹ���
// ����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
// IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
// IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr (GPIOA_BASE + 12) // 0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) // 0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) // 0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) // 0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE + 12) // 0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE + 12) // 0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE + 12) // 0x40011E0C

#define GPIOA_IDR_Addr (GPIOA_BASE + 8) // 0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8) // 0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8) // 0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8) // 0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE + 8) // 0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE + 8) // 0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE + 8) // 0x40011E08

// IO�ڲ���,ֻ�Ե�һ��IO��!
// ȷ��n��ֵС��16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) // ���
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)  // ����

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) // ���
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)  // ����

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) // ���
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)  // ����

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) // ���
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)  // ����

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) // ���
#define PEin(n) BIT_ADDR(GPIOE_IDR_Addr, n)  // ����

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) // ���
#define PFin(n) BIT_ADDR(GPIOF_IDR_Addr, n)  // ����

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) // ���
#define PGin(n) BIT_ADDR(GPIOG_IDR_Addr, n)  // ����



// ����Ϊ��ຯ��
#ifdef __cplusplus
extern "C"
{
#endif
    void WFI_SET(void);      // ִ��WFIָ��
    void INTX_DISABLE(void); // �ر������ж�
    void INTX_ENABLE(void);  // ���������ж�
    void MSR_MSP(u32 addr);  // ���ö�ջ��ַ
#ifdef __cplusplus
}
#endif

template <class _TypeT>
class OSAllocator
{
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef _TypeT value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    OSAllocator() noexcept {}

    OSAllocator(const OSAllocator&) noexcept {}

    template <class _TypeU>
    struct rebind {
        typedef OSAllocator<_TypeU> other;
    };

    template <class _TypeU>
    OSAllocator(const OSAllocator<_TypeU>&) noexcept {}

    template <class _TypeU>
    OSAllocator& operator=(const OSAllocator<_TypeU>&) noexcept {
        return *this;
    }

    pointer address(reference __x) const {
        return &__x;
    }

    const_pointer address(const_reference __x) const {
        return &__x;
    }

    pointer allocate(size_type __n, const void* = 0) {
        if (__n == 0) return nullptr;
        pointer __p = static_cast<pointer>(pvPortMalloc(__n * sizeof(value_type)));
        // if (!__p) throw std::bad_alloc();
        return __p;
    }

    void deallocate(pointer __p, size_type) {
        vPortFree(__p);
    }

    size_type max_size() const noexcept {
        return size_type(-1) / sizeof(value_type);
    }

    template <class _Up, class... _Args>
	void construct(_Up* __p, _Args... __args) {
		::new((void *)__p) _Up(__args...);
	}

    template <class _Up>
    void destroy(_Up* __p) noexcept {
        __p->~_Up();
    }
};
