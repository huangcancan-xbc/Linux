#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

int counter = 0;                        // 全局变量

void* task(void* arg)                   // 所有线程都会调用这个函数
{
    // 每个线程都对同一个 counter 加 1
    for (int i = 0; i < 5; ++i)
    {
        ++counter;
        cout << "线程 " << pthread_self() << " counter = " << counter << endl;
        sleep(1);
    }

    pthread_exit(nullptr);
}

int main()
{
    pthread_t t1, t2;

    pthread_create(&t1, nullptr, task, nullptr);
    pthread_create(&t2, nullptr, task, nullptr);

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    cout << "最终counter = " << counter << endl;
    return 0;
}