#include <iostream>
#include <unistd.h>
#include <ctime>
#include "RingQueue.hpp"
#include "Task.hpp"
using namespace std;

// 线程数据结构，传递给线程函数
struct ThreadData
{
    RingQueue<Task> *rq;                                             // 环形队列指针
    string threadname;                                               // 线程名称
};

// 生产者线程函数
void *Productor(void *args)
{
    ThreadData *td = static_cast<ThreadData*>(args);                 // 获取线程数据
    RingQueue<Task> *rq = td->rq;                                    // 获取环形队列
    string name = td->threadname;                                    // 获取线程名称
    int len = opers.size();                                          // 操作符个数

    while (true)                                                     // 持续生产
    {
        // 1. 获取数据：生成随机任务
        int data1 = rand() % 10 + 1;                                 // 生成[1,10]随机数
        usleep(10);                                                  // 微秒延迟
        int data2 = rand() % 10;                                     // 生成[0,9]随机数
        char op = opers[rand() % len];                               // 随机选择操作符
        Task t(data1, data2, op);                                    // 创建任务对象

        // 2. 生产数据：将任务放入环形队列
        rq->Push(t);                                                 // 生产任务
        
        // 修复打印格式，添加换行符使输出更清晰
        cout << name << " 生产任务：" << t.get_task() << endl;

        sleep(1);                                                    // 1秒生产间隔
    }
    return nullptr;
}

// 消费者线程函数
void *Consumer(void *args)
{
    ThreadData *td = static_cast<ThreadData*>(args);                 // 获取线程数据
    RingQueue<Task> *rq = td->rq;                                    // 获取环形队列
    string name = td->threadname;                                    // 获取线程名称

    while (true)                                                     // 持续消费
    {
        // 1. 消费数据：从环形队列取出任务
        Task t;                                                      // 创建任务对象
        rq->Pop(&t);                                                 // 消费任务（阻塞等待）
       
        // 2. 处理数据：执行任务并输出结果
        t();                                                         // 执行运算
        
        // 修复打印格式，添加换行符使输出更清晰
        cout << name << " 消费任务：" << t.get_task() << " 结果：" << t.get_ret() << endl;
    }
    return nullptr;
}

int main()
{
    cout << "=== 环形队列生产者-消费者模型 ===" << endl;
    
    srand(time(nullptr) ^ getpid());                                 // 设置随机种子
    RingQueue<Task> *rq = new RingQueue<Task>(10);                   // 创建容量为10的环形队列（小一点便于测试）

    pthread_t consumers[3], producers[2];                            // 创建多个消费者和生产者

    // 创建2个生产者线程
    cout << "创建 2 个生产者线程..." << endl;
    for (int i = 0; i < 2; i++)
    {
        ThreadData *td = new ThreadData();                           // 创建线程数据
        td->rq = rq;                                                 // 设置环形队列
        td->threadname = "[生产者-" + to_string(i) + "]";            // 设置线程名称

        pthread_create(&producers[i], nullptr, Productor, td);       // 创建生产者线程
    }
    
    // 创建3个消费者线程
    cout << "创建 3 个消费者线程..." << endl;
    for (int i = 0; i < 3; i++)
    {
        ThreadData *td = new ThreadData();                           // 创建线程数据
        td->rq = rq;                                                 // 设置环形队列
        td->threadname = "[消费者-" + to_string(i) + "]";             // 设置线程名称

        pthread_create(&consumers[i], nullptr, Consumer, td);        // 创建消费者线程
    }
    
    // 等待线程结束（实际程序不会退出）
    for (int i = 0; i < 2; i++)
    {
        pthread_join(producers[i], nullptr);                         // 等待生产者线程
    }
    for (int i = 0; i < 3; i++)
    {
        pthread_join(consumers[i], nullptr);                         // 等待消费者线程
    }

    delete rq;                                                       // 释放内存
    return 0;
}