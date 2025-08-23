#include "BlockingQueue.hpp"
#include "Task.hpp"
#include <unistd.h>
#include <ctime>
#include <sys/time.h>

const string ops = "+-*/%";                                          // 操作符
BlockingQueue<Task>* TaskQueue;                                      // 全局任务队列指针
pthread_mutex_t g_print_mutex = PTHREAD_MUTEX_INITIALIZER;           // 全局打印互斥锁，确保输出尽量不交错

// 获取当前时间戳（毫秒），用于标识消息的产生时间
long long get_timestamp()
{
    struct timeval tv;                                               // 时间结构体
    gettimeofday(&tv, nullptr);                                      // 获取当前时间
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;                     // 转换为毫秒时间戳
}

// 线程安全的打印函数，添加时间戳便于观察执行顺序
void safe_print(const string& message)
{
    pthread_mutex_lock(&g_print_mutex);                              // 加锁，防止输出交错
    cout << "[" << get_timestamp() << "] " << message << endl;       // 带时间戳的输出
    pthread_mutex_unlock(&g_print_mutex);                            // 解锁
}

// 消费者线程函数
void* consumer(void* arg)
{
    int id = *(int*)arg;                                             // 获取线程ID

    while(true)                                                      // 持续消费
    {
        Task task = TaskQueue->pop();                                // 从队列取出任务（可能阻塞等待）
        task();                                                      // 执行任务运算
        
        // 构造消费完成的消息
        string s = "[消费者 " + to_string(id) + "] 完成了任务：" + task.get_task() + "，结果为：" + task.get_ret();
        safe_print(s);                                               // 线程安全的打印，带时间戳
        usleep(500000);                                              // 500ms延迟，控制消费速度
    }

    pthread_exit(nullptr);                                           // 线程退出
}

// 生产者线程函数
void* producer(void* arg)
{
    int id = *(int*)arg;                                             // 获取线程ID
    srand(time(nullptr) + id * 1000);                                // 设置随机种子，避免重复

    while(true)                                                      // 持续生产
    {
        int x = rand() % 50 + 1;                                     // 生成随机数 [1, 50]，范围更小便于观察
        int y = rand() % 50 + 1;                                     // 生成随机数 [1, 50]
        char op = ops[rand() % ops.size()];                          // 随机选择操作符

        Task task(x, y, op);                                         // 创建任务对象

        // 先打印生产消息，再放入队列
        string s = "[生产者 " + to_string(id) + "] 生产了任务：" + task.get_task();
        safe_print(s);                                               // 线程安全的打印，带时间戳
        
        TaskQueue->push(task);                                       // 将任务放入队列（可能阻塞等待）
        usleep(1000000);                                             // 1秒延迟，控制生产速度
    }

    pthread_exit(nullptr);                                           // 线程退出
}

int main()
{
    TaskQueue = new BlockingQueue<Task>(5);                          // 创建容量为5的阻塞队列

    int ids[5] = { 1, 2, 3, 4, 5 };                                  // 线程ID数组

    // 创建2个消费者线程
    cout << "创建 2 个消费者线程" << endl;
    pthread_t Consumers[2];
    for(int i = 0; i < 2; i++)
    {
        pthread_create(&Consumers[i], nullptr, consumer, &ids[i]);   // 创建消费者线程
    }
    
    // 创建3个生产者线程
    cout << "创建 3 个生产者线程" << endl;
    pthread_t Producers[3];
    for(int i = 0; i < 3; i++)
    {
        pthread_create(&Producers[i], nullptr, producer, &ids[i+2]); // 创建生产者线程
    }
    
    // 等待所有线程结束（实际程序不会退出）
    for(int i = 0; i < 2; i++)
    {
        pthread_join(Consumers[i], nullptr);                         // 等待消费者线程
    }
    for(int i = 0; i < 3; i++)
    {
        pthread_join(Producers[i], nullptr);                         // 等待生产者线程
    }
    
    delete TaskQueue;                                                // 释放队列内存
    return 0;
}