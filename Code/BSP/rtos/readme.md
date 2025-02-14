## 基于FreRTOS的CPP支持包：RTOS

### 信号量semaphore
在FreeRTOS中支持四种类型的信号量，分别是：
- 二进制信号量
- 互斥信号量
- 递归互斥信号量
- 计数型信号量
在bsp中，四种信号量均以**模板类**`semaphore`进行封装，在定义和初始化信号量并实例化模板时，通过使用不同的`tag`定义信号量类型，如下所示：

```cpp
struct binary_semaphore_tag{ };     //二进制信号量
struct mutex_semaphore_tag{ };      //互斥信号量
struct recursive_semaphore_tag{ };  //递归互斥信号量
struct counting_semaphore_tag{ };   //计数型信号量
```
以定义并释放一个互斥信号量为例，使用本bsp有两种方法为：
1. 直接使用`semaphore`
```cpp
using namespace RTOS;
semaphore<mutex_semaphore_tag> mtx;
mtx.give();
```
2. 使用`mutex`类，`mutex`实际上是继承于`semaphore`实现的
```cpp
using namespace RTOS;
mutex mtx;
mtx.unlock();
```
若直接使用FreeRTOS原本的API，则需要以下步骤
```c
SemaphoreHandle_t mtx = xSemaphoreCreateMutex();
xSemaphoreGive(mtx);
```
