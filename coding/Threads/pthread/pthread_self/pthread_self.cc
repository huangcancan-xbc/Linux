#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

void* func(void* arg)
{
    pthread_t tid = pthread_self();         // 获取当前线程 ID
    cout << "子线程 pthread_self() = " << tid << endl;
    printf("这是子线程 pthread_t 变量的地址 %p\n", &tid);
    return nullptr;
}

int main()
{
    pthread_t tid;

    pthread_create(&tid, nullptr, func, nullptr);

    sleep(1);

    pthread_t main_tid = pthread_self();    // 主线程自身 ID
    cout << "主线程 pthread_self() = " << main_tid << endl;
    printf("这是主线程 pthread_t 变量的地址 %p\n", &main_tid);

    return 0;
}
