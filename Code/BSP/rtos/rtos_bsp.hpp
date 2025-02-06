#pragma once
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "event_groups.h"
#include <vector>
using namespace std;
namespace RTOS
{

    typedef enum
    {
        osFALSE = pdFALSE,
        osTURE = pdTRUE,
    } OS_State;

    class Thread
    {
    public:
        Thread(void (*task_code)(void *p_arg), const char *name, uint32_t priority) : __task_code(task_code)
        {
            xTaskCreate(task_code, name, __starck_size, nullptr, priority, __handler);
        }
        Thread(void (*task_code)(void *p_arg), const char *name, uint32_t priority,TaskHandle_t *handler) : __task_code(task_code)
        {
            xTaskCreate(task_code, name, __starck_size, nullptr, priority, handler);
        }

        Thread(void (*task_code)(void *p_arg), const char *name, uint32_t priority, size_t starck_size) : __task_code(task_code), __starck_size(starck_size)
        {
            xTaskCreate(task_code, name, __starck_size, nullptr, priority, __handler);
        }

    private:
        void (*__task_code)(void *p_arg);
        size_t __starck_size = 128;
        TaskHandle_t *__handler;
    };

    void os_start_scheduler();
    uint32_t os_ms_to_ticks(uint32_t nms);

    /* queue */
    typedef QueueHandle_t OS_Queue;
    OS_Queue os_queue_create(uint32_t queue_length, uint32_t item_size);
    OS_State os_queue_recv(OS_Queue os_queue, uint8_t *pxdata, uint32_t ticks_to_wait);
    OS_State os_queue_send(OS_Queue os_queue, uint8_t *pxdata, uint32_t ticks_to_wait);
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
    OS_EventBits os_event_sync(OS_Event event, const OS_EventBits bits_to_set, const OS_EventBits bits_wait_for, uint32_t ticks_to_wait);

    // std::vector
}
