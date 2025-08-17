#include <pthread.h>
#include <iostream>
#include <unistd.h>
using namespace std;

void* func(void* arg)
{
    sleep(1);
    cout << "子线程开始执行..." << endl;
    cout << "子线程执行完毕，自动释放资源。" << endl;
    pthread_exit(nullptr);
}

int main()
{
    pthread_t tid;

    // 创建线程（默认是 joinable 状态）
    if (pthread_create(&tid, nullptr, func, nullptr) != 0)
    {
        perror("线程创建失败");
        return 1;
    }

    // 设置线程为分离状态
    if (pthread_detach(tid) != 0)
    {
        perror("线程分离失败");
        return 1;
    }

    cout << "主线程不等待子线程，直接结束。" << endl;

    sleep(3);
    return 0;
}