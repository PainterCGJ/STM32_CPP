#include "rtos_bsp.hpp"
namespace RTOS
{

    void os_start_scheduler()
    {
        vTaskStartScheduler();
        while (1)
            ;
    }

    uint32_t os_ms_to_ticks(uint32_t nms)
    {
        return pdMS_TO_TICKS(nms);
    }

    void *os_malloc(size_t size)
    {
        return pvPortMalloc(size);
    }

    void os_free(void *pv)
    {
        vPortFree(pv);
    }

    OS_Queue os_queue_create(uint32_t queue_length, uint32_t item_size)
    {
        return xQueueCreate(queue_length, item_size);
    }

    OS_State os_queue_recv(OS_Queue os_queue, uint8_t *pxdata, uint32_t ticks_to_wait)
    {
        if (xPortIsInsideInterrupt() == pdTRUE)
        {
            OS_State os_sta;
            BaseType_t xTaskWokenByReceive = pdFALSE;
            os_sta = (OS_State)xQueueReceiveFromISR(os_queue, pxdata, &xTaskWokenByReceive);
            if (xTaskWokenByReceive != pdFALSE)
            {
                /* We should switch context so the ISR returns to a different task.
           NOTE: How this is done depends on the port you are using. Check
           the documentation and examples for your port. */
                taskYIELD();
            }
            return os_sta;
        }
        return (OS_State)xQueueReceive(os_queue, pxdata, ticks_to_wait);
    }

    OS_State os_queue_send(OS_Queue os_queue, uint8_t *pxdata, uint32_t ticks_to_wait)
    {
        if (xPortIsInsideInterrupt() == pdTRUE)
        {
            OS_State os_sta;
            BaseType_t xTaskWokenByReceive = pdFALSE;
            os_sta = (OS_State)xQueueSendFromISR(os_queue, pxdata, &xTaskWokenByReceive);
            if (xTaskWokenByReceive != pdFALSE)
            {
                /* We should switch context so the ISR returns to a different task.
           NOTE: How this is done depends on the port you are using. Check
           the documentation and examples for your port. */
                taskYIELD();
            }
            return os_sta;
        }
        return (OS_State)xQueueSend(os_queue, pxdata, ticks_to_wait);
    }

    OS_State os_queue_overwrite(OS_Queue os_queue, uint8_t *pxdata)
    {
        if (xPortIsInsideInterrupt() == pdTRUE)
        {
            OS_State os_sta;
            BaseType_t xTaskWokenByReceive = pdFALSE;
            os_sta = (OS_State)xQueueOverwriteFromISR(os_queue, pxdata, &xTaskWokenByReceive);
            if (xTaskWokenByReceive != pdFALSE)
            {
                /* We should switch context so the ISR returns to a different task.
           NOTE: How this is done depends on the port you are using. Check
           the documentation and examples for your port. */
                taskYIELD();
            }
            return os_sta;
        }
        return (OS_State)xQueueOverwrite(os_queue, pxdata);
    }

    void os_queue_index(OS_Queue os_queue, uint32_t index_base_tail)
    {
    }

    void os_queue_delet(OS_Queue os_queue)
    {
        vQueueDelete(os_queue);
    }

    OS_List os_list_create(void)
    {
        OS_List list = (OS_List)os_malloc(sizeof(List_t));
        if (list == NULL)
        {
            return NULL;
        }
        vListInitialise(list);
        return list;
    }

    void os_list_item_init(OS_ListItem *pxItem, void *pxOwner)
    {
        vListInitialiseItem(pxItem);
        listSET_LIST_ITEM_OWNER(pxItem, pxOwner);
    }

    void os_list_insert_end(OS_List list, OS_ListItem *pxItem)
    {
        vListInsertEnd(list, pxItem);
    }

    void os_list_printf(OS_List list)
    {
        OS_ListItem *pxItem = (OS_ListItem *)(list->xListEnd.pxNext);
        printf("\r\n================================\r\n");
        printf("List:               0x%p\r\n", list);
        printf("Number of item:     %d\r\n", list->uxNumberOfItems);
        printf("xListEnd:           0x%p\r\n", &list->xListEnd);
        printf("xListEnd->pxNext:   0x%p\r\n", list->xListEnd.pxNext);

        uint8_t i = 0;
        // while (pxItem != (OS_ListItem *)(&list->xListEnd))
        List_Index_Loop(pxItem)
        {
            printf("\r\n--------------------------------\r\n");
            printf("order:          [%d]\r\n", i++);
            printf("pxItem:         0x%x\r\n", pxItem);
            printf("xItemValue:     %d\r\n", pxItem->xItemValue);
            printf("pxPrevious:     0x%p\r\n", pxItem->pxPrevious);
            printf("pxContainer:    0x%p\r\n", pxItem->pxContainer);
            printf("pvOwner:        0x%p\r\n", pxItem->pvOwner);
            printf("pxNext:         0x%p\r\n", pxItem->pxNext);
            // pxItem = pxItem->pxNext;
        }
        printf("\r\n================================\r\n");
    }

    uint32_t os_list_remove_item(OS_ListItem *pxItem)
    {
        return uxListRemove(pxItem);
    }

    void os_delet_list_item(OS_ListItem *pxItem, OS_State free_owner)
    {
        if (pxItem->pxContainer != NULL)
        {
            /*this list item still in the list*/
            os_list_remove_item(pxItem);
        }

        if (free_owner == osTURE)
        {
            os_free(pxItem->pvOwner);
        }

        os_free(pxItem);
    }

    void os_delet_list(OS_List list, OS_State free_item, OS_State free_owner)
    {
        if (free_item == osTURE)
        {
            OS_ListItem *pxItem = list->xListEnd.pxNext;
            OS_ListItem *pxNext;
            while ((void *)pxItem != (void *)(&list->xListEnd))
            {
                pxNext = pxItem->pxNext;
                os_delet_list_item(pxItem, free_owner);
                pxItem = pxNext;
            }
        }
        os_free(list);
    }

    OS_Event os_event_create(void)
    {
        return xEventGroupCreate();
    }

    OS_EventBits os_set_event_bits(OS_Event event, OS_EventBits bits_to_set)
    {
        if (xPortIsInsideInterrupt() == pdTRUE)
        {
            OS_EventBits event_bits;
            BaseType_t xTaskWokenByReceive = pdFALSE;
            event_bits = (OS_State)xEventGroupSetBitsFromISR(event, bits_to_set, &xTaskWokenByReceive);
            if (xTaskWokenByReceive != pdFALSE)
            {
                /* We should switch context so the ISR returns to a different task.
           NOTE: How this is done depends on the port you are using. Check
           the documentation and examples for your port. */
                taskYIELD();
            }
            return event_bits;
        }
        return (OS_EventBits)xEventGroupSetBits(event, bits_to_set);
    }

    OS_EventBits os_clear_event_bits(OS_Event event, OS_EventBits bits_to_clear)
    {
        if (xPortIsInsideInterrupt() == pdTRUE)
        {
            return (OS_EventBits)xEventGroupClearBitsFromISR(event, bits_to_clear);
        }
        return (OS_EventBits)xEventGroupClearBits(event, bits_to_clear);
    }

    OS_EventBits os_get_event_bits(OS_Event event)
    {
        if (xPortIsInsideInterrupt() == pdTRUE)
        {
            OS_EventBits event_bits;
            BaseType_t xTaskWokenByReceive = pdFALSE;
            event_bits = (OS_EventBits)xEventGroupGetBitsFromISR(event);
            if (xTaskWokenByReceive != pdFALSE)
            {
                /* We should switch context so the ISR returns to a different task.
           NOTE: How this is done depends on the port you are using. Check
           the documentation and examples for your port. */
                taskYIELD();
            }
            return event_bits;
        }
        return (OS_EventBits)xEventGroupGetBits(event);
    }

    OS_EventBits os_event_sync(OS_Event event, const OS_EventBits bits_to_set, const OS_EventBits bits_wait_for, uint32_t ticks_to_wait)
    {
        if (xPortIsInsideInterrupt() == pdFAIL)
        {
            return xEventGroupSync(event, bits_to_set, bits_wait_for, ticks_to_wait);
        }
        return 0;
    }

    uint32_t os_lock(void)
    {
        if (xPortIsInsideInterrupt() == pdTRUE)
        {
            return taskENTER_CRITICAL_FROM_ISR();
        }
        else
        {
            taskENTER_CRITICAL();
        }
        return 0;
    }

    void os_unlock(uint32_t key)
    {
        if (xPortIsInsideInterrupt() == pdTRUE)
        {
            taskEXIT_CRITICAL_FROM_ISR(key);
        }
        else
        {
            taskEXIT_CRITICAL();
        }
    }

    void task_run(void *parm)
    {
        Thread *thread = static_cast<Thread *>(parm);
        thread->thread_code(); //运行线程代码
        thread->kill();      //结束线程
    }
}

extern void rtos_main(void);
void system_config(void);

static void __main(void *parg)
{
    rtos_main();
}
TaskHandle_t MainHandler;

int main(void)
{
    system_config();
    xTaskCreate(__main, "main", 1280, nullptr, 0, &MainHandler);
    RTOS::os_start_scheduler();
}
/* end of file */
