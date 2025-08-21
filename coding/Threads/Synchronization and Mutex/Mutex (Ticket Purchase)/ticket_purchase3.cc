#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <unistd.h>
using namespace std;

#define NUM 5                                              // 定义线程数量
int tickets = 10000;                                       // 全局共享变量：票数
pthread_mutex_t ticket_mutex;                              // 动态互斥锁（全局）

class thread_data
{
public:
    thread_data(int number)                                // 构造函数
    {
        thread_name = "线程" + to_string(number);
    }
public:
    string thread_name;                                    // 线程名称
};

void* getticket(void* arg)
{
    thread_data* data = static_cast<thread_data*>(arg);
    const char* name = data->thread_name.c_str();

    while(true)
    {
        pthread_mutex_lock(&ticket_mutex);                 // 加锁

        if(tickets > 0)
        {
            usleep(1000);                                  // 模拟延时
            
            printf("%s 获得 1 张票，剩余 %d 张票\n", name, tickets);
            tickets--;
        }
        else
        {
            pthread_mutex_unlock(&ticket_mutex);           // 解锁防止死锁
            break;
        }

        pthread_mutex_unlock(&ticket_mutex);               // 解锁
    }

    cout << data->thread_name << "执行结束！" << endl;
    return nullptr;
}

int main()
{
    // 初始化动态互斥锁（不使用属性）
    if(pthread_mutex_init(&ticket_mutex, nullptr) != 0)
    {
        cerr << "互斥锁初始化失败！" << endl;
        return -1;
    }

    vector<pthread_t> tids;
    vector<thread_data*> thread_datas;

    cout << "多线程买票演示程序" << endl;
    cout << "总票数: " << tickets << " 张" << endl;
    cout << "创建 " << NUM << " 个线程同时买票..." << endl << endl;

    for(int i = 0; i < NUM; i++)
    {
        pthread_t tid;
        thread_data* data = new thread_data(i);
        thread_datas.push_back(data);

        if(pthread_create(&tid, nullptr, getticket, data) != 0)
        {
            cerr << "线程创建失败：" << i << endl;
            delete data;
            continue;
        }
        
        tids.push_back(tid);
    }

    cout << "所有线程创建完成，开始并发买票..." << endl;

    // 等待所有线程执行完毕
    for (size_t i = 0; i < tids.size(); ++i)
    {
        if(pthread_join(tids[i], nullptr) != 0)
        {
            cerr << "等待线程 " << i << " 失败！" << endl;
        }
    }

    cout << "\n最终剩余票数: " << tickets << " 张" << endl;
    cout << "买票结束！" << endl;

    // 释放线程数据内存
    for(auto data : thread_datas)
    {
        delete data;
    }

    // 销毁互斥锁
    pthread_mutex_destroy(&ticket_mutex);

    return 0;
}