#pragma once
#include <iostream>
#include <pthread.h>
#include <string>
#include <ctime>
using namespace std;

typedef void (*callback_t)();                                        // 无参数无返回值的函数指针
static int thread_num = 0;                                           // 全局线程编号计数器

class Thread
{
public:
    static void* thread_func(void* arg)                              // 线程函数
    {
        Thread* thread = static_cast<Thread*>(arg);                  // 转换为线程指针
        thread->Enter_callback();

    }
public:
    Thread(callback_t cb)
        :tid_(0),                                                    // 初始化线程ID为0
        name_(""),                                                   // 初始化线程名称为空
        start_timestamp_(0),                                         // 初始化启动时间戳为0
        isrunning_(false),                                           // 初始化运行状态为false
        cb_(cb)                                                      // 保存回调函数
    {

    }

    ~Thread()                                                        // 析构函数
    {

    }

    void run()                                                       // 启动线程
    {
        name_ = "thread-" + to_string(thread_num++);                 // 设置线程名称
        start_timestamp_ = time(nullptr);                            // 记录启动时间戳
        isrunning_ = true;                                           // 设置运行状态为true
        pthread_create(&tid_, nullptr, thread_func, this);           // 创建线程
    }

    void Enter_callback()
    {
        cb_();                                                       // 执行回调函数
    }

    bool is_runing()                                                 // 判断线程是否运行
    {
        return isrunning_;                                           // 返回运行状态
    }

    uint64_t start_timestamp()                                       // 获取启动时间戳
    {
        return start_timestamp_;                                     // 返回启动时间戳
    }

    string name()                                                    // 获取线程名称
    {
        return name_;                                                // 返回线程名称
    }

    void join()                                                      // 等待线程结束
    {
        pthread_join(tid_, nullptr);                                 // 等待线程结束
        isrunning_ = false;                                          // 设置运行状态为false
    }

private:
    pthread_t tid_;                                                  // 线程ID
    string name_;                                                    // 线程名称
    uint64_t start_timestamp_;                                       // 启动时间戳
    bool isrunning_;                                                 // 运行状态标志

    callback_t cb_;                                                  // 回调函数指针
};