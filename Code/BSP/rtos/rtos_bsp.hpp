#pragma once
#include "FreeRTOS.h"
#include "event_groups.h"
#include "list.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include <cstring>
#include <iterator>

using namespace std;

namespace RTOS
{

#define Default_Starck_size 128
#define OS_WAIT_FOREVER portMAX_DELAY
#define List_Index_Loop(item) for (OS_ListItem *__end = (OS_ListItem *)listGET_END_MARKER(item->pxContainer); item != __end; item = item->pxNext)

    // 手动实现 std::enable_if
    template <bool B, typename T = void>
    struct enable_if
    {
    };

    template <typename T>
    struct enable_if<true, T>
    {
        using type = T;
    };

    // 手动实现 std::is_same
    template <typename T, typename U>
    struct is_same
    {
        static const bool value = false;
    };

    template <typename T>
    struct is_same<T, T>
    {
        static const bool value = true;
    };

    typedef enum
    {
        osFALSE = pdFALSE,
        osTURE = pdTRUE,
    } OS_State;

    typedef TaskHandle_t OS_Threat;

    void task_run(void *parm);

    /**
     * @brief 启动调度器
     */
    void os_start_scheduler();

    /**
     * @brief 将毫秒单位转化为心跳
     * @param[in] nms 毫秒值
     * @return  nms毫秒对应的心跳值
     */
    uint32_t os_ms_to_ticks(uint32_t nms);

    /* queue */
    typedef QueueHandle_t OS_Queue;
    OS_Queue os_queue_create(uint32_t queue_length, uint32_t item_size);
    OS_State os_queue_recv(OS_Queue os_queue, uint8_t *pxdata,
                           uint32_t ticks_to_wait);
    OS_State os_queue_send(OS_Queue os_queue, uint8_t *pxdata,
                           uint32_t ticks_to_wait);
    OS_State os_queue_overwrite(OS_Queue os_queue, uint8_t *pxdata);
    void os_queue_delet(OS_Queue os_queue);

    /* list */
    typedef List_t *OS_List;
    typedef ListItem_t OS_ListItem;
    OS_List os_list_create(void);
    void os_list_item_init(OS_ListItem *pxItem, void *pxOwner);
    void os_list_insert_end(OS_List list, OS_ListItem *pxItem);
    uint32_t os_list_remove_item(OS_ListItem *pxItem);
    void os_delet_list_item(OS_ListItem *pxItem, OS_State free_owner);
    void os_delet_list(OS_List list, OS_State free_item, OS_State free_owner);
    void os_list_printf(OS_List list);

    /* event group */
    typedef EventGroupHandle_t OS_Event;
    typedef EventBits_t OS_EventBits;
    OS_Event os_event_create(void);
    OS_EventBits os_set_event_bits(OS_Event event, OS_EventBits bits_to_set);
    OS_EventBits os_clear_event_bits(OS_Event event, OS_EventBits bits_to_clear);
    OS_EventBits os_get_event_bits(OS_Event event);
    OS_EventBits os_event_sync(OS_Event event, const OS_EventBits bits_to_set,
                               const OS_EventBits bits_wait_for,
                               uint32_t ticks_to_wait);

    /* semaphore */
    typedef enum
    {
        BinarySemaphoreTag,
        MutexSemaphoreTag,
        RecursiveMutexSemaphoreTag
    } Semaphore_tag;
    typedef SemaphoreHandle_t OS_Semaphore;
    OS_Semaphore os_semaphore_creat(Semaphore_tag tag);
    OS_Semaphore os_counting_semaphore_creat(uint32_t max_count, uint32_t initial_count);
    OS_State os_semaphore_take(OS_Semaphore semaphore, uint32_t ticks_to_wait);
    OS_State os_semaphore_give(OS_Semaphore semaphore);
    OS_State os_recursive_semaphore_take(OS_Semaphore semaphore, uint32_t ticks_to_wait);
    OS_State os_recursive_semaphore_give(OS_Semaphore semaphore);

    /* cpu */
    uint32_t os_cpu_lock(void);
    void os_cpu_unlock(uint32_t key);

    class Thread
    {
    public:
        /**
         * @brief 创建线程，线程无传入参数，可用于直接创建线程对象
         * @param[in] task_code 线程代码
         * @param[in] name 线程名称
         * @param[in] priority 线程优先值
         * @param[in] starck_size 堆栈大小，默认值为Default_Starck_size
         * @example
         * void task(void* parg)
         * {
         *      printf("hello world\r\n");
         *      //运行结束，自动删除线程
         * }
         * 
         * void rtos_main(void)
         * {
         *      Thread TASK(task, "task", 1);
         *      task.join();
         *      while(1){}
         * }
         */
        Thread(void (*task_code)(void *p_arg), const char *name, uint32_t priority, size_t starck_size = Default_Starck_size)
            : __task_code(task_code), __starck_size(starck_size)
        {
            strcpy(__name, name);
        }

        /**
         * @brief 创建线程，线程有传入参数，可用于直接创建线程对象
         * @param[in] task_code 线程代码
         * @param[in] p_arg 线程传入参数
         * @param[in] name 线程名称
         * @param[in] priority 线程优先值
         * @param[in] starck_size 堆栈大小，默认值为Default_Starck_size
         * @example
         * void task(void* parg)
         * {
         *      printf("a = %d", *((int*)(parg)));
         *      //运行结束，自动删除线程
         * }
         * 
         * void rtos_main(void)
         * {
         *      int a = 10;
         *      Thread TASK(task, (void*)(&a), "task", 1);
         *      task.join();
         *      while(1){}
         * }
         */
        Thread(void (*task_code)(void *p_arg), void *p_arg, const char *name, uint32_t priority, size_t starck_size = Default_Starck_size)
            : __task_code(task_code), __p_arg(p_arg), __starck_size(starck_size)
        {
            strcpy(__name, name);
        }

        /**
         * @brief 当选择使用Thread作为基类定义派生类时，无需传入task_code线程代码
         * 需要在派生类中定义虚函数task_code，作为线程运行的主体，并在调用join后才
         * 真正的创建该线程，执行task_code里的内容。当task_code结束后，该线程将会被
         * 自动删除，即FreeRTOS删除任务
         * @param[in] name 线程名称
         * @param[in] starck_size 栈大小
         * @param[in] priority 线程优先值
         * @example
         * class TASK: public Thread
         * {
         *  public:
         *  TASK()：Thread("task",1,64){join();}
         *  virtual void task_code() override
         *  {
         *      printf("hello world\r\n");
         *  }
         * }
         */
        Thread(const char *name, uint32_t priority, size_t starck_size) : __priority(priority), __starck_size(starck_size)
        {
            strcpy(__name, name);
        }

        /**
         * @brief 创建并启动线程
         */
        void join()
        {
            if (!__join_flag)
            {
                __join_flag = 1;
                xTaskCreate(task_run, __name, __starck_size, this, __priority, &__handler);
            }
        }

        /**
         * @brief 挂起线程，无传入参数时挂起自身线程
         * @param[in] handler 需要挂起的线程句柄
         */
        void suspend(OS_Threat handler = nullptr)
        {
            vTaskSuspend((TaskHandle_t)handler);
        }

        /**
         * @brief 恢复线程
         * @param[in] handler 需要挂起的线程句柄
         */
        void resume(OS_Threat handler)
        {
            vTaskResume((TaskHandle_t)handler);
        }

        /**
         * @brief 删除线程
         * @param[in] handler 需要删除的线程句柄
         */
        void kill(OS_Threat handler = nullptr)
        {
            vTaskDelete((TaskHandle_t)__handler);
        }

        /**
         * @brief 获取线程句柄
         * @return 线程句柄
         */
        OS_Threat get_thread_handler()
        {
            return __handler;
        }

        virtual void thread_code()
        {
            __task_code(__p_arg);
        }

    private:
        void (*__task_code)(void *p_arg);
        void *__p_arg;
        char __name[configMAX_TASK_NAME_LEN + 1];
        uint32_t __priority;
        size_t __starck_size;
        OS_Threat __handler;
        uint8_t __join_flag = 0;
    };

    template <typename _Type>
    class queue
    {
    public:
        /**
         * @brief 构造队列
         * @param[in] queue_length 队列容量，指能容纳元素的个数
         */
        queue(uint32_t queue_length)
        {
            __handler = os_queue_create(queue_length, sizeof(_Type));
        }

        /**
         * @brief 元素入队，遇到阻塞默认一直等待
         * @param[in] elm_to_push 需要入队的元素
         * @param[in] ticks_to_wait 阻塞时等待时间
         * @return osFALSE 入队失败；pdTRUE 入队成功
         */
        OS_State push(const _Type &elm_to_push, uint32_t ticks_to_wait = OS_WAIT_FOREVER)
        {
            return os_queue_send(__handler, (uint8_t *)(&elm_to_push), ticks_to_wait);
        }

        /**
         * @brief 取出元素，遇到阻塞默认一直等待
         * @param[in] elm_recv 接收取出的元素
         * @param[in] ticks_to_wait 阻塞时等待时间
         * @return osFALSE 队列为空，取出失败；pdTRUE 取出成功
         */
        OS_State pop(_Type &elm_recv, uint32_t ticks_to_wait = OS_WAIT_FOREVER)
        {
            return os_queue_recv(__handler, (uint8_t *)(&elm_recv), ticks_to_wait);
        }

        /**
         * @brief 查询当前队列中所含的元素个数
         * @return 当前队列中所含的元素个数
         */
        uint32_t size() { return uxQueueMessagesWaiting(__handler); }

        ~queue() { os_queue_delet(__handler); }

    private:
        OS_Queue __handler;
    };

    template <typename _Type = int>
    class list
    {
    public:
        list()
        {
            __handle = os_list_create();
        }

        ~list()
        {
            os_delet_list(__handle, osTURE, osTURE);
        }

        void push_back(const _Type &elm_to_push)
        {
            _Type *__elm = new _Type;
            OS_ListItem *__item = new OS_ListItem;
            std::memcpy(__elm, &elm_to_push, sizeof(_Type));
            os_list_item_init(__item, __elm);
            os_list_insert_end(__handle, __item);
        }

        _Type &front()
        {
            return *((_Type *)(((OS_ListItem *)listGET_HEAD_ENTRY(__handle))->pvOwner));
        }

        _Type &back()
        {
            return *((_Type *)((__handle->xListEnd.pxPrevious->pvOwner)));
        }

        void pop_front()
        {
            OS_ListItem *__item = (OS_ListItem *)listGET_HEAD_ENTRY(__handle);
            os_list_remove_item(__item);
            os_delet_list_item(__item, osTURE);
        }

        void pop_back()
        {
            OS_ListItem *__item = __handle->xListEnd.pxPrevious;
            os_list_remove_item(__item);
            os_delet_list_item(__item, osTURE);
        }

        void remove(const _Type &__elm)
        {
            OS_ListItem *__item = (OS_ListItem *)listGET_HEAD_ENTRY(__handle);
            OS_ListItem *__next;
            List_Index_Loop(__item)
            {
                if (std::memcmp(&__elm, __item->pvOwner, sizeof(_Type)) == 0)
                {
                    __next = (OS_ListItem *)__item->pxNext;
                    os_delet_list_item(__item, osTURE);
                    __item = __next;
                }
            }
        }

        void print()
        {
            os_list_printf(__handle);
        }

        uint32_t size()
        {
            return __handle->uxNumberOfItems;
        }

        class __list_iterator : public std::iterator<std::forward_iterator_tag, _Type>
        {
        private:
            OS_ListItem *_current;

        public:
            __list_iterator(OS_ListItem *item = nullptr) : _current(item) {}

            // 解引用操作符
            _Type &operator*() const
            {
                return *reinterpret_cast<_Type *>(_current->pvOwner);
            }

            // 成员访问操作符
            _Type *operator->() const
            {
                return reinterpret_cast<_Type *>(_current->pvOwner);
            }

            // 前置递增操作符
            __list_iterator &operator++()
            {
                _current = reinterpret_cast<OS_ListItem *>(listGET_NEXT(_current));
                return *this;
            }

            // 后置递增操作符
            __list_iterator operator++(int)
            {
                __list_iterator tmp = *this;
                ++(*this); //利用前置递增操作符
                return tmp;
            }

            // 相等性比较
            bool operator==(const __list_iterator &other) const
            {
                return _current == other._current;
            }

            // 不等性比较
            bool operator!=(const __list_iterator &other) const
            {
                return _current != other._current;
            }
        };

        // 返回指向链表开头的迭代器
        __list_iterator begin() const
        {
            return __list_iterator(reinterpret_cast<OS_ListItem *>(listGET_HEAD_ENTRY(__handle)));
        }

        // 返回指向链表末尾的迭代器
        __list_iterator end() const
        {
            return __list_iterator(reinterpret_cast<OS_ListItem *>(&(__handle->xListEnd)));
        }

    private:
        OS_List __handle;
    };

    class event
    {

    public:
        event() : __handler(os_event_create()) {}
        ~event()
        {
            vEventGroupDelete(__handler);
        }
        OS_EventBits set(OS_EventBits bits_to_set)
        {
            return os_set_event_bits(__handler, bits_to_set);
        }

        OS_EventBits clear(OS_EventBits bits_to_clear)
        {
            return os_clear_event_bits(__handler, bits_to_clear);
        }

        OS_EventBits get()
        {
            return os_get_event_bits(__handler);
        }

        OS_EventBits wait(OS_EventBits bits_to_wait, uint32_t ticks_to_wait = OS_WAIT_FOREVER, OS_State wait_for_all = osTURE)
        {
            return xEventGroupWaitBits(__handler, bits_to_wait, osFALSE, wait_for_all, ticks_to_wait);
        }

        OS_EventBits sync(OS_EventBits bits_to_set, OS_EventBits bits_wait_for, uint32_t ticks_to_wait = OS_WAIT_FOREVER)
        {
            return os_event_sync(__handler, bits_to_set, bits_wait_for, ticks_to_wait);
        }

    private:
        OS_Event __handler;
    };

    struct binary_semaphore_tag
    {
    };
    struct mutex_semaphore_tag
    {
    };
    struct recursive_semaphore_tag
    {
    };
    struct counting_semaphore_tag
    {
    };

#define ENABLE_IF_SAME(T, Tag) typename RTOS::enable_if<RTOS::is_same<T, Tag>::value, int>::type

    template <typename T, typename Tag>
    using EnableIfSame = typename RTOS::enable_if<RTOS::is_same<T, Tag>::value, int>::type;

    template <typename tag = binary_semaphore_tag>
    class semaphore
    {
    public:
        template <typename T = tag, EnableIfSame<T, binary_semaphore_tag> = 0>
        semaphore() : __handler(os_semaphore_creat(BinarySemaphoreTag)) {}

        template <typename T = tag, EnableIfSame<T, mutex_semaphore_tag> = 0>
        semaphore() : __handler(os_semaphore_creat(MutexSemaphoreTag)) {}

        template <typename T = tag, EnableIfSame<T, recursive_semaphore_tag> = 0>
        semaphore() : __handler(os_semaphore_creat(RecursiveMutexSemaphoreTag)) {}

        template <typename T = tag, EnableIfSame<T, recursive_semaphore_tag> = 0>
        OS_State give()
        {
            return os_recursive_semaphore_give((SemaphoreHandle_t)__handler);
        }

        template <typename T = tag, EnableIfSame<T, recursive_semaphore_tag> = 0>
        OS_State take(uint32_t ticks_to_wait = OS_WAIT_FOREVER)
        {
            return os_recursive_semaphore_take((SemaphoreHandle_t)__handler, ticks_to_wait);
        }

        template <typename T = tag, EnableIfSame<T, counting_semaphore_tag> = 0>
        semaphore(uint32_t max_count, uint32_t initial_count) : __handler(os_counting_semaphore_creat(max_count, initial_count)) {}

        template <typename T = tag,
                  typename RTOS::enable_if<!RTOS::is_same<T, recursive_semaphore_tag>::value, int>::type = 0>
        OS_State give()
        {
            return os_semaphore_give(__handler);
        }

        template <typename T = tag,
                  typename RTOS::enable_if<!RTOS::is_same<T, recursive_semaphore_tag>::value, int>::type = 0>
        OS_State take(uint32_t ticks_to_wait = OS_WAIT_FOREVER)
        {
            return os_semaphore_take(__handler, ticks_to_wait);
        }

        ~semaphore()
        {
            vSemaphoreDelete(__handler);
        }

    private:
        OS_Semaphore __handler;
    };

    class mutex : private semaphore<mutex_semaphore_tag>
    {
    public:
        mutex() : semaphore<mutex_semaphore_tag>() { unlock(); }
        OS_State unlock() { return give(); }
        OS_State lock(uint32_t ticks_to_wait = OS_WAIT_FOREVER) { return take(ticks_to_wait); }
    };

    class recursive_mutex : private semaphore<recursive_semaphore_tag>
    {
    public:
        recursive_mutex() : semaphore<recursive_semaphore_tag>() { unlock(); }
        OS_State unlock() { return give(); }
        OS_State lock(uint32_t ticks_to_wait = OS_WAIT_FOREVER) { return take(ticks_to_wait); }
    };

    // std::vector
} // namespace RTOS
