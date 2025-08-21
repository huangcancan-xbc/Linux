#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <unistd.h>
using namespace std;

#define NUM 5                                              // 定义线程数量
int tickets = 100;                                         // 全局共享变量：票数（多个线程将同时访问这个变量，临界资源）

class thread_data
{
public:
    thread_data(int number)                                // 构造函数：根据线程编号初始化线程名称
    {
        thread_name = "线程" + to_string(number);
    }
public:
    string thread_name;                                    // 线程名称
};

void* getticket(void* arg)
{
    thread_data* data = static_cast<thread_data*>(arg);    // 将传入的参数转换为threadData类型指针
    const char* name = data->thread_name.c_str();          // 获取线程名称

    while(true)                                            // 无限循环买票，竞态条件发生在这里！
    {
        if(tickets > 0)                                    // 票数大于 0 时，买票
        {
            usleep(1000);                                  // 模拟票务系统延时

            printf("%s 获得 1 张票，剩余 %d 张票\n", name, tickets);    // 打印票数信息
            tickets--;                                     // 票数减 1
        }
        else                                               // 票数不足时，跳出循环
        {
            break;
        }
    }

    cout << "线程" << name << "执行结束！" << endl;
    return nullptr;
}

int main()
{
    vector<pthread_t> tids;                                // 存储线程ID的向量容器
    vector<thread_data*> thread_datas;                     // 存储线程数据的向量容器

    cout << "多线程买票演示程序" << endl;
    cout << "总票数: " << tickets << " 张" << endl;
    cout << "创建 " << NUM << " 个线程同时买票..." << endl << endl;

    for(int i = 0; i < NUM; i++)
    {
        pthread_t tid;                                      // 线程ID变量
        thread_data* data = new thread_data(i);             // 为每个线程创建独立的数据对象
        thread_datas.push_back(data);                       // 保存数据对象指针

        pthread_create(&tid, nullptr, getticket, data);     // 创建线程，传入参数为线程数据对象指针
        tids.push_back(tid);                                // 保存线程ID
    }

    cout << "所有线程创建完成，开始并发买票..." << endl;
    sleep(3);                                               // 等待 3 秒，让主进程等待子进程结束

    for(auto data : thread_datas)
    {
        delete data;                                        // 释放每个线程的数据对象
    }

    return 0;
}