#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
using namespace std;

sem_t sem;

void* worker(void* arg)
{
    int id = *(int*)arg;

    sem_wait(&sem);                        // 请求信号量（如果值为0，则阻塞等待）
    cout << "Thread：" << id << " 获取到了信号量" << endl;

    sleep(1);                              // 模拟工作

    sem_post(&sem);                        // 释放信号量
    cout << "Thread：" << id << " 释放了信号量" << endl;
    
    return nullptr;
}

int main()
{
    sem_init(&sem, 0, 3);                   // 初始化信号量，最多允许3个线程同时进入

    pthread_t threads[10];
    int ids[10];
    for(int i = 0; i < 10; i++)
    {
        ids[i] = i;
        pthread_create(&threads[i], nullptr, worker, &ids[i]);
    }

    for(int i = 0; i < 10; i++)
    {
        pthread_join(threads[i], nullptr);
    }

    sem_destroy(&sem);                      // 清理资源
    return 0;
}