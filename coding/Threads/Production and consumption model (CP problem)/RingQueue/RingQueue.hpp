#pragma once
#include <iostream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

const static int default_capacity = 5;                               // 环形队列默认容量

template <class T>
class RingQueue
{
private:
    // P操作：等待信号量（资源减少）
    void P(sem_t& sem)
    {
        sem_wait(&sem);                                              // 等待信号量，原子操作
    }

    // V操作：释放信号量（资源增加）
    void V(sem_t& sem)
    {
        sem_post(&sem);                                              // 释放信号量，原子操作
    }

    // 加锁操作
    void Lock(pthread_mutex_t& mutex)
    {
        pthread_mutex_lock(&mutex);                                  // 加锁（获取互斥锁），原子操作
    }

    // 解锁操作
    void Unlock(pthread_mutex_t& mutex)
    {
        pthread_mutex_unlock(&mutex);                                // 解锁（释放互斥锁），原子操作
    }

public:
    RingQueue(int capacity = default_capacity)
        :ringqueue_(capacity),
        capacity_(capacity),
        c_index_(0),
        p_index_(0)
    {
        sem_init(&cdata_sem_, 0, 0);                                 // 初始化消费者数据信号量为0（初始无数据）
        sem_init(&pspace_sem_, 0, capacity_);                        // 初始化生产者空间信号量为cap（初始全部空间）
    
        pthread_mutex_init(&c_mutex_, nullptr);                      // 初始化消费者互斥锁
        pthread_mutex_init(&p_mutex_, nullptr);                      // 初始化生产者互斥锁
    }

    ~RingQueue()
    {
        sem_destroy(&cdata_sem_);                                    // 销毁消费者数据信号量
        sem_destroy(&pspace_sem_);                                   // 销毁生产者空间信号量

        pthread_mutex_destroy(&c_mutex_);                            // 销毁消费者互斥锁
        pthread_mutex_destroy(&p_mutex_);                            // 销毁生产者互斥锁
    }

    // 生产者：向环形队列添加元素
    void Push(const T& in)
    {
        P(pspace_sem_);                                              // 等待可用空间（空间资源-1）

        Lock(p_mutex_);                                              // 加锁保护生产者写入位置
        ringqueue_[p_index_] = in;                                   // 在生产者位置写入数据

        // 位置后移，维持环形特性
        p_index_++;                                                  // 生产者下标后移
        p_index_ %= capacity_;                                       // 取模实现环形
        Unlock(p_mutex_);                                            // 解锁

        V(cdata_sem_);                                               // 增加数据资源（唤醒消费者）
    }

    // 消费者：从环形队列取出元素
    void Pop(T* out)
    {
        P(cdata_sem_);                                               // 等待可用数据（数据资源-1）

        Lock(c_mutex_);                                              // 加锁保护消费者读取位置
        *out = ringqueue_[c_index_];                                 // 从消费者位置读取数据

        // 位置后移，维持环形特性
        c_index_++;                                                  // 消费者下标后移
        c_index_ %= capacity_;                                       // 取模实现环形
        Unlock(c_mutex_);                                            // 解锁

        V(pspace_sem_);                                              // 增加空间资源（唤醒生产者）
    }

private:
    vector<T> ringqueue_;                                            // 存储数据的环形数组
    int capacity_;                                                   // 队列容量

    int c_index_;                                                    // 消费者当前位置下标
    int p_index_;                                                    // 生产者当前位置下标

    sem_t cdata_sem_;                                                // 消费者关注的数据资源信号量（有多少数据可消费）
    sem_t pspace_sem_;                                               // 生产者关注的空间资源信号量（有多少空间可生产）

    pthread_mutex_t c_mutex_;                                        // 消费者互斥锁，保护消费者位置
    pthread_mutex_t p_mutex_;                                        // 生产者互斥锁，保护生产者位置
};
