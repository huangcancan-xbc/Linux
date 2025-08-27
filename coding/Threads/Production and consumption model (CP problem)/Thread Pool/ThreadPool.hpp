#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <pthread.h>
#include <unistd.h>
using namespace std;

// 线程信息结构体：保存每个线程的基本信息
struct ThreadInfo
{
    pthread_t tid;                                                   // 线程ID
    string name;                                                     // 线程名称
};

static const int default_num = 5;                                    // 默认线程数量

template <class T>
class ThreadPool
{
public:
    // 加锁操作：保护共享资源不被多个线程同时访问
    void Lock()
    {
        pthread_mutex_lock(&mutex_);                                 // 获取互斥锁
    }
    
    // 解锁操作：释放互斥锁，让其他线程可以访问共享资源
    void Unlock()
    {
        pthread_mutex_unlock(&mutex_);                               // 释放互斥锁
    }
    
    // 唤醒一个等待的线程：当有新任务时唤醒空闲的线程
    void Wakeup()
    {
        pthread_cond_signal(&cond_);                                 // 唤醒一个等待的线程
    }
    
    // 线程睡眠等待：让线程进入等待状态，节省CPU资源
    void ThreadSleep()
    {
        pthread_cond_wait(&cond_, &mutex_);                          // 等待条件变量，会自动释放锁
    }
    
    // 判断任务队列是否为空
    bool IsQueueEmpty()
    {
        return tasks_.empty();                                       // 返回任务队列是否为空
    }
    
    // 根据线程ID获取线程名称：用于日志输出，知道是哪个线程在工作
    string GetThreadName(pthread_t tid)
    {
        for (const auto &ti : threads_)
        {
            if (ti.tid == tid)
                return ti.name;
        }

        return "Unknown";                                            // 找不到返回未知
    }

public:
    // 线程执行函数：每个工作线程都会执行这个函数
    static void *HandlerTask(void *args)
    {
        ThreadPool<T> *tp = static_cast<ThreadPool<T> *>(args);      // 获取线程池对象指针
        string name = tp->GetThreadName(pthread_self());             // 获取当前线程的名字
        
        while (true)                                                 // 线程一直运行，不会退出
        {
            tp->Lock();                                              // 加锁保护任务队列
            
            // 等待任务：如果任务队列为空，线程就睡觉等待
            while (tp->IsQueueEmpty())
            {
                cout << name << " 等待任务..." << endl;
                tp->ThreadSleep();                                   // 睡觉等待新任务
            }
            
            T t = tp->Pop();                                         // 从队列取出一个任务
            tp->Unlock();                                            // 解锁，让其他线程可以访问队列

            t();                                                     // 执行任务（调用任务的operator()）
            cout << name << " 运行任务，结果是：" << t.get_result() << endl;
        }
        
        return nullptr;
    }
    
    // 启动线程池：创建所有的工作线程
    void Start()
    {
        int num = threads_.size();                                   // 获取要创建的线程数量
        cout << "启动 " << num << " 个线程..." << endl;
        
        for (int i = 0; i < num; i++)
        {
            threads_[i].name = "[线程 " + to_string(i + 1) + "]";    // 给每个线程起名字
            // 创建线程：&threads_[i].tid保存线程ID，HandlerTask是线程要执行的函数，this是传给函数的参数
            pthread_create(&(threads_[i].tid), nullptr, HandlerTask, this);
            cout << "创建线程 " << threads_[i].name << endl;
        }
    }
    
    // 从任务队列取出任务
    T Pop()
    {
        T t = tasks_.front();                                        // 取出队列第一个任务
        tasks_.pop();                                                // 从队列中删除这个任务
        return t;                                                    // 返回任务
    }
    
    // 向任务队列添加任务：外部程序调用这个函数来提交任务
    void Push(const T &t)
    {
        Lock();                                                      // 加锁
        tasks_.push(t);                                              // 把新任务加入队列
        Wakeup();                                                    // 唤醒一个睡觉的线程来处理任务
        Unlock();                                                    // 解锁
    }
    
    // 获取线程池单例实例：保证整个程序只有一个线程池对象
    static ThreadPool<T> *GetInstance()
    {
        // 第一次检查：如果已经创建了实例，就直接返回（不需要加锁，提高性能）
        if (nullptr == tp_)                                          
        {
            pthread_mutex_lock(&lock_);                              // 加锁：防止多个线程同时创建实例
            // 第二次检查：再次确认还没有创建实例（防止在等待锁的时候其他线程已经创建了）
            if (nullptr == tp_)                                      
            {
                cout << "log：单例线程池首次创建完成！" << endl;
                tp_ = new ThreadPool<T>();                           // 创建唯一的线程池实例
            }
            pthread_mutex_unlock(&lock_);                            // 解锁
        }

        return tp_;                                                  // 返回线程池实例
    }

private:
    // 私有构造函数：防止外部直接创建对象，只能通过GetInstance获取
    ThreadPool(int num = default_num) : threads_(num)
    {
        pthread_mutex_init(&mutex_, nullptr);                        // 初始化互斥锁
        pthread_cond_init(&cond_, nullptr);                          // 初始化条件变量
    }
    
    // 私有析构函数：防止外部随意销毁线程池
    ~ThreadPool()
    {
        pthread_mutex_destroy(&mutex_);                              // 销毁互斥锁
        pthread_cond_destroy(&cond_);                                // 销毁条件变量
    }
    
    // 禁用拷贝构造函数：防止线程池对象被复制（因为线程不能被复制）
    // ThreadPool tp1; ThreadPool tp2 = tp1; // 这样就不允许
    ThreadPool(const ThreadPool<T> &) = delete;
    
    // 禁用赋值操作符：防止线程池对象被赋值（因为线程不能被赋值）
    // ThreadPool tp1, tp2; tp2 = tp1; // 这样就不允许
    const ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;

private:
    vector<ThreadInfo> threads_;                                     // 保存所有工作线程的信息
    queue<T> tasks_;                                                 // 任务队列：存放等待执行的任务

    pthread_mutex_t mutex_;                                          // 互斥锁：保护任务队列不被多个线程同时修改
    pthread_cond_t cond_;                                            // 条件变量：用于线程间的等待和唤醒

    // 单例指针：指向唯一的线程池实例
    // static修饰：这个变量属于类本身，不属于任何对象，所有对象共享这一个变量
    static ThreadPool<T> *tp_;                                       
    
    // 单例保护锁：保护创建单例实例的过程，防止多个线程同时创建
    // static修饰：这个锁属于类本身，用于保护静态成员变量tp_
    static pthread_mutex_t lock_;                                    
};

// 下面是静态成员变量定义：必须在类外单独定义，这些变量在程序启动时就存在，不依赖于任何对象

// 开始时为空，第一次调用GetInstance时才会创建
template <class T>
ThreadPool<T> *ThreadPool<T>::tp_ = nullptr;                         

// 使用PTHREAD_MUTEX_INITIALIZER初始化
template <class T>
pthread_mutex_t ThreadPool<T>::lock_ = PTHREAD_MUTEX_INITIALIZER;    