#pragma once
#include <iostream>
#include <queue>
#include <pthread.h>
using namespace std;

template <class T>
class BlockingQueue
{
private:
    queue<T> q_;                                                     // STL队列，存储实际数据
    int max_capacity;                                                // 队列最大容量
    pthread_mutex_t mutex_;                                          // 互斥锁，保护共享资源
    pthread_cond_t not_empty_;                                       // 非空条件变量，消费者等待
    pthread_cond_t not_full_;                                        // 非满条件变量，生产者等待

public:
    static const int DEFAULT_MAX_CAPACITY = 10;                      // 队列默认初始容量

    // 构造函数：初始化队列和同步变量
    BlockingQueue(int max_capacity = DEFAULT_MAX_CAPACITY)
        :max_capacity(max_capacity)
    {
        pthread_mutex_init(&mutex_, nullptr);                        // 初始化互斥锁
        pthread_cond_init(&not_empty_, nullptr);                     // 初始化非空条件变量
        pthread_cond_init(&not_full_, nullptr);                      // 初始化非满条件变量
    }

    // 析构函数：清理资源
    ~BlockingQueue()
    {
        pthread_mutex_destroy(&mutex_);                              // 销毁互斥锁
        pthread_cond_destroy(&not_empty_);                           // 销毁非空条件变量
        pthread_cond_destroy(&not_full_);                            // 销毁非满条件变量
    }
    
    // 生产者添加元素进行入队
    void push(const T& item)
    {
        pthread_mutex_lock(&mutex_);                                 // 加锁，保护共享资源
        
        // 队列已满，等待非满条件变量
        // 使用while防止虚假唤醒
        while (q_.size() >= max_capacity)
        {
            pthread_cond_wait(&not_full_, &mutex_);                  // 等待队列有空间
        }

        q_.push(item);                                               // 入队
        pthread_cond_signal(&not_empty_);                            // 唤醒等待的消费者线程
        pthread_mutex_unlock(&mutex_);                               // 解锁
    }

    // 消费者取出元素进行出队
    T pop()
    {
        pthread_mutex_lock(&mutex_);                                 // 加锁

        // 队列空则等待
        // 使用while防止虚假唤醒
        while (q_.empty())
        {
            pthread_cond_wait(&not_empty_, &mutex_);                 // 等待队列有数据
        }

        T item = q_.front();                                         // 获取队首元素
        q_.pop();                                                    // 出队

        pthread_cond_signal(&not_full_);                             // 唤醒等待的生产者线程
        pthread_mutex_unlock(&mutex_);                               // 解锁

        return item;                                                 // 返回元素
    }
};