#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

void* func(void* arg)
{
    int id = *(int*)arg;                    // 获取线程编号
    sleep(id);                              // 模拟不同耗时
    int* result = new int(id * 10);         // 所有线程都要分配内存，让他们分别乘以 10

    cout << "子线程 " << id << " 完成任务，返回结果 = " << *result << endl;
    return (void*)result;                   // 返回退出码（指针）
}

int main()
{
    pthread_t tids[5];                      // 线程 ID 数组
    int ids[5];                             // 用于传参的线程编号数组

    for (int i = 0; i < 5; i++)             // 创建多个子线程
    {
        ids[i] = i + 1;                     // 编号从 1 开始
        pthread_create(&tids[i], nullptr, func, &ids[i]);
    }

    for (int i = 0; i < 5; i++)             // 主线程等待所有子线程完成
    {
        void* retval = nullptr;
        pthread_join(tids[i], &retval);     // 阻塞等待子线程退出

        int* res = (int*)retval;            // 转换返回值
        cout << "主线程获取到线程 " << (i + 1) << " 的返回结果 = " << *res << endl;
        delete res;                         // 释放返回结果的内存
    }

    cout << "所有线程任务已完成，主线程退出。" << endl;
    return 0;
}