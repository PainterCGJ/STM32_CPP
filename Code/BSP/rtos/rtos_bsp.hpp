#pragma once
#include "FreeRTOS.h"
#include "event_groups.h"
#include "list.h"
#include "queue.h"
#include "task.h"
#include <vector>
// #include <list>

using namespace std;
namespace RTOS
{

#define Default_Starck_size 128
#define OS_WAIT_FOREVER portMAX_DELAY

    typedef enum
    {
        osFALSE = pdFALSE,
        osTURE = pdTRUE,
    } OS_State;

    /**
     * @brief 启动调度器
     */
    void os_start_scheduler();

    /**
     * @brief 将心毫秒单位转化为心跳
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

    /* cpu */
    uint32_t os_lock(void);
    void os_unlock(uint32_t key);

    class Thread
    {
    public:
        /**
         * @brief 创建线程，线程的栈大小为默认值
         * @param[in] task_code 线程代码
         * @param[in] name 线程名称
         * @param[in] priority 线程优先值
         */
        Thread(void (*task_code)(void *p_arg), const char *name, uint32_t priority) : __task_code(task_code)
        {
            xTaskCreate(task_code, name, __starck_size, nullptr, priority, &__handler);
        }

        /**
         * @brief 创建线程
         * @param[in] task_code 线程代码
         * @param[in] name 线程名称
         * @param[in] starck_size 栈大小
         * @param[in] priority 线程优先值
         */
        Thread(void (*task_code)(void *p_arg), const char *name, uint32_t priority, size_t starck_size) : __task_code(task_code), __starck_size(starck_size)
        {
            xTaskCreate(task_code, name, __starck_size, nullptr, priority, &__handler);
        }

        void kill()
        {
            vTaskDelete(__handler);
        }

    private:
        void (*__task_code)(void *p_arg);
        size_t __starck_size = Default_Starck_size;
        TaskHandle_t __handler;
        // static std::list<TaskHandle_t> __thread_list;
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
         * @brief 元素入队，遇阻塞不等待
         * @param[in] elm_to_push 需要入队的元素
         * @return osFALSE 入队失败；pdTRUE 入队成功
         */
        OS_State push(const _Type &elm_to_push)
        {
            return os_queue_send(__handler, (uint8_t *)(&elm_to_push), 0);
        }

        /**
         * @brief 元素入队
         * @param[in] elm_to_push 需要入队的元素
         * @param[in] ticks_to_wait 阻塞时等待时间
         * @return osFALSE 入队失败；pdTRUE 入队成功
         */
        OS_State push(const _Type &elm_to_push, uint32_t ticks_to_wait)
        {
            return os_queue_send(__handler, (uint8_t *)(&elm_to_push), ticks_to_wait);
        }

        /**
         * @brief 取出元素，遇阻塞不等待
         * @param[in] elm_recv 接收取出的元素
         * @return osFALSE 队列为空，取出失败；pdTRUE 取出成功
         */
        OS_State pop(_Type &elm_recv)
        {
            return os_queue_recv(__handler, (uint8_t *)(&elm_recv), 0);
        }

        /**
         * @brief 取出元素
         * @param[in] elm_recv 接收取出的元素
         * @param[in] ticks_to_wait 阻塞时等待时间
         * @return osFALSE 队列为空，取出失败；pdTRUE 取出成功
         */
        OS_State pop(_Type &elm_recv, uint32_t ticks_to_wait)
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

    template <typename _Type=int>
    class list
    {
    public:
        list()
        {
            __handle = os_list_create();
        }

        void push_back(_Type &elm_to_push)
        {
            _Type *__elm = new _Type;
            OS_ListItem* __item = new OS_ListItem;
            std::memcpy(__elm, &elm_to_push, sizeof(_Type));
            os_list_item_init(__item,__elm);
            os_list_insert_end(__handle,__item);
        }

        _Type& front()
        {
            return *((_Type*)(((OS_ListItem*)listGET_HEAD_ENTRY(__handle))->pvOwner));
        }

        _Type& back()
        {
            return *((_Type*)((__handle->xListEnd.pxPrevious->pvOwner)));
        }

        void print()
        {
            os_list_printf(__handle);
        }

        uint32_t size()
        {
            return __handle->uxNumberOfItems;
        }

    private:
        OS_List __handle;
    };
    // std::vector
} // namespace RTOS
