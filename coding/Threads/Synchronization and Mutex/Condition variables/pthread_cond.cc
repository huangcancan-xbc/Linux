#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
using namespace std;

// uint64_t: 64位无符号整数类型，用于确保能存储64位的线程ID或大数值
// uintptr_t: 足够大以容纳指针值的无符号整数类型，用于安全的指针到整数转换

int cnt = 0;                                            // 临界资源，全局计数器，多个线程共享访问
int tid_cnt = 5;                                        // 线程数量
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      // 互斥锁，保护临界资源的访问，防止数据竞争
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;         // 条件变量，用于线程间通信，控制线程的等待和唤醒

void* Count(void* arg)
{
    pthread_detach(pthread_self());                     // 分离线程，这样线程结束时会自动回收资源
    uint64_t number = (uint64_t)(uintptr_t)arg;         // 修复指针到整数的转换警告，两次类型转换确保安全
    cout << "[创建]线程" << number << "创建成功！" << endl;

    while(true)
    {
        // 1. 加锁进入临界区
        pthread_mutex_lock(&mutex);                     // 加锁
        cout << "[等待]线程" << number << "获取锁，准备等待信号！" << endl;

        // 这里隐含了一个判断："是否收到了主线程的信号？"
        // 如果没有收到信号，临界资源状态就是"不就绪"，所以线程选择等待

        // 2. 等待条件满足
        cout << "[阻塞]线程" << number << "调用pthread_cond_wait，即将释放锁并进入等待！" << endl;
        pthread_cond_wait(&cond, &mutex);               // 等待信号（临界资源就绪）
        // pthread_cond_wait的内部工作原理：
        // a. 释放mutex锁（让其他线程可以获取锁）
        // b. 将当前线程加入等待队列并阻塞
        // c. 等待被pthread_cond_signal或pthread_cond_broadcast唤醒
        // d. 被唤醒后自动重新获取mutex锁
        // e. 返回（此时线程重新持有锁）

        cout << "[唤醒]线程" << number << "被唤醒并重新获得锁！开始处理任务！" << endl;

        // 3. 被唤醒后执行的代码，此时已经重新获得了锁，可以安全地访问临界资源cnt
        cout << "[执行]线程" << number << "收到信号，执行任务！cnt = " << cnt++ << endl;

        // 4. 解锁 - 离开临界区
        pthread_mutex_unlock(&mutex);
        cout << "[完成]线程" << number << "释放锁，任务完成，重新进入等待循环中！" << endl;
    }

    pthread_exit(nullptr);                              // 线程结束，释放资源（显式）
    // return nullptr;                                  // 线程结束，释放资源（隐式）
}

void Demo_cond_init()                                   // 这个函数没有实际意义，只是为了演示条件变量的初始化
{
    pthread_cond_t cond1;

    // 方式1: 静态初始化（推荐用于全局变量）
    // pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    // 方式2: 动态初始化（推荐用于局部变量）
    int result = pthread_cond_init(&cond1, nullptr);
    if (result != 0)
    {
        cerr << "初始化条件变量失败！" << strerror(result) << endl;     // cerr: 标准错误输出流，用于输出错误信息
    }

    pthread_cond_destroy(&cond1);                      // 使用完后需要销毁
}

int main()
{
    Demo_cond_init();

    for(uint64_t i = 0; i < tid_cnt; i++)
    {
        pthread_t tid;
        int result = pthread_create(&tid, nullptr, Count, (void*)(uintptr_t)i);
        if(result != 0)
        {
            cerr << "创建线程失败！" << strerror(result) << endl;
        }

        usleep(1000);
    }

    sleep(3);                                           // 等待所有线程进入等待状态
    cout << "主线程控制开始！" << endl;
    cout << "控制规则：每1秒唤醒一个线程，每3秒唤醒全部线程！" << endl;

    int count = 0;
    // while(count < 自定义value)                        // 限制循环次数，避免无限运行
    while(true)
    {
        sleep(1);
        count++;

        if (count % 3 == 0)
        {
            // 使用pthread_cond_broadcast唤醒所有等待的线程
            cout << "[广播]唤醒所有线程！" << endl;
            pthread_cond_broadcast(&cond);
        }
        else
        {
            // 使用pthread_cond_signal唤醒一个等待的线程
            cout << "[单播]唤醒一个线程！" << endl;
            pthread_cond_signal(&cond);
        }

        cout << "信号已经发出！count = " << count << endl;

        // 显示当前计数器值
        pthread_mutex_lock(&mutex);
        cout << "当前计数值：" << cnt << endl;
        pthread_mutex_unlock(&mutex);
    }

    cout << "主线程结束！" << endl << "最终计数值：" << cnt << endl;

    return 0;
}