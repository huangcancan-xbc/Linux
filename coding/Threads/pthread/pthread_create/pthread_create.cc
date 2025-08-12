#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

void* func(void* arg)
{
    char* str = (char*)arg;
    while(1)
    {
        cout << str << endl;
        sleep(1);
    }
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, nullptr, func, (void*)"我是子线程，ID是：123456");

    while(1)
    {
        cout << "我是主线程!" << endl;
        sleep(2);
    }

    return 0;
}