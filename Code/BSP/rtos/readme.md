## ����FreRTOS��CPP֧�ְ���RTOS

### �ź���semaphore
��FreeRTOS��֧���������͵��ź������ֱ��ǣ�
- �������ź���
- �����ź���
- �ݹ黥���ź���
- �������ź���
��bsp�У������ź�������**ģ����**`semaphore`���з�װ���ڶ���ͳ�ʼ���ź�����ʵ����ģ��ʱ��ͨ��ʹ�ò�ͬ��`tag`�����ź������ͣ�������ʾ��

```cpp
struct binary_semaphore_tag{ };     //�������ź���
struct mutex_semaphore_tag{ };      //�����ź���
struct recursive_semaphore_tag{ };  //�ݹ黥���ź���
struct counting_semaphore_tag{ };   //�������ź���
```
�Զ��岢�ͷ�һ�������ź���Ϊ����ʹ�ñ�bsp�����ַ���Ϊ��
1. ֱ��ʹ��`semaphore`
```cpp
using namespace RTOS;
semaphore<mutex_semaphore_tag> mtx;
mtx.give();
```
2. ʹ��`mutex`�࣬`mutex`ʵ�����Ǽ̳���`semaphore`ʵ�ֵ�
```cpp
using namespace RTOS;
mutex mtx;
mtx.unlock();
```
��ֱ��ʹ��FreeRTOSԭ����API������Ҫ���²���
```c
SemaphoreHandle_t mtx = xSemaphoreCreateMutex();
xSemaphoreGive(mtx);
```
