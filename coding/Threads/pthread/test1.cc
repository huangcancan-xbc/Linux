#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
using namespace std;

void* thread1(void*)
{
    cout << "[线程1] 正常 return 退出" << endl;               // 方法1：函数 return 自然返回
    return (void*)1;                                         // 返回退出值 1
    // return nullptr;
}

void* thread2(void*)
{
    cout << "[线程2] 使用 pthread_exit 主动退出" << endl;     // 方法2：显式调用 pthread_exit
    pthread_exit((void*)2);                                  // 返回退出值 2
}

void* thread3(void*)
{
    cout << "[线程3] 进入无限循环，等待被取消..." << endl;     // 方法3：无限循环，等待 cancel
    while (true)
    {
        sleep(1);                                            // 可取消点
    }
    return nullptr;
}

void* thread4(void*)                                         // 方法4：使用 exit 退出（注意：此会导致整个进程退出！不推荐！）
{
    cout << "[线程4] 调用了 exit(3)，整个进程都会终止！" << endl;
    exit(3);                                                 // 会结束整个程序（不推荐在线程中用）
}

int main()
{
    pthread_t t1, t2, t3, t4;
    void* ret;

    // 创建四个线程
    pthread_create(&t1, nullptr, thread1, nullptr);
    pthread_create(&t2, nullptr, thread2, nullptr);
    pthread_create(&t3, nullptr, thread3, nullptr);
    // pthread_create(&t4, nullptr, thread4, nullptr);

    // 等待线程1退出（正常 return）
    pthread_join(t1, &ret);
    cout << "[主线程] 线程1退出，退出值 = " << (long long)ret << "（return 返回）" << endl;

    // 等待线程2退出（pthread_exit）
    pthread_join(t2, &ret);
    cout << "[主线程] 线程2退出，退出值 = " << (long long)ret << "（pthread_exit 返回）" << endl;

    // 取消线程3
    sleep(2);                       // 等它跑一下
    pthread_cancel(t3);
    pthread_join(t3, &ret);
    if (ret == PTHREAD_CANCELED)
    {
        cout << "[主线程] 线程3被取消（pthread_cancel）" << endl;
    }
    else
    {
        cout << "[主线程] 线程3退出值 = " << (long long)ret << endl;
    }

    sleep(2);                       // 等它跑一下
    // 等待线程4退出（exit）=> 会导致其他线程无法正常退出，整个进程终止！
    // pthread_join(t4, &ret);
    // cout << "[主线程] 线程4退出，退出值 = " << (long long)ret << "（exit 终止）" << endl;

    return 0;
}