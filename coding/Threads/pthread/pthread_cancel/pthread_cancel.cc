#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

void* func(void* arg)
{
    while (true)
    {
        cout << "子线程工作中..." << endl;
        sleep(1);                   // sleep 是一个可取消点
        // pthread_testcancel();       // 手动设置一个取消点
        // 注意：如果使用 pthread_testcancel()，在这个程序中会无限打印 "子线程工作中..."，在3秒后会自动退出，
        // 和sleep(1)一样的效果。虽然看起来像是死循环，但实际上是可以取消的！
    }

    return nullptr;
}

int main()
{
    pthread_t tid;
    void* retval;

    pthread_create(&tid, nullptr, func, nullptr);
    sleep(3);                       // 让子线程运行一会

    pthread_cancel(tid);            // 发送取消请求
    pthread_join(tid, &retval);     // 等待子线程结束并获取退出码

    if (retval == PTHREAD_CANCELED)
    {
        cout << "收到取消请求，子线程被取消了！" << endl;
    }

    return 0;
}